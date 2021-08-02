static bool s_running = false;
static bool s_verbose = false;
static bool s_veryVerbose = false;
static bool s_dryRun = false;

#include "udevconnection.h"
#include "utils.h"
#include "keys.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>


extern "C" {
#include <linux/input.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <unistd.h>
#include <wordexp.h>
#include <signal.h>
#include <termios.h>
}

static bool s_pressedKeys[KEY_CNT];
static bool s_usedKeys[KEY_CNT];

struct File
{
    File (const std::string &filename, bool keyboard, const int flags = O_RDONLY | O_NONBLOCK | O_CLOEXEC) : m_filename(filename) {
        fd = open(filename.c_str(), flags);

        if (fd == -1) {
            perror(("Failed to open " + filename).c_str());
            return;
        }

        if (keyboard) {
            long bits[KEY_CNT] = { 0 };
            int ret = ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(bits)), bits);
            if (ret < 0) {
                perror(("Failed to get key bits from " + filename).c_str());
                close(fd);
                fd = -1;
                return;
            }
        }

        if (s_verbose) printf("Opened %s\n", m_filename.c_str());
    }

    ~File() {
        if (fd != -1) {
            // Closing is very slow, for some reason, so print progress
            std::cout << "Closing " << fd << std::flush;
            close(fd);
            if (s_verbose) {
                puts("");
            } else {
                std::cout << "\033[2K\r";
            }
        }
    }

    File(File&& other) : m_filename(std::move(other.m_filename)) {
        fd = other.fd;
        other.fd = -1;
    }

    File &operator=(File &&other) {
        m_filename = std::move(other.m_filename);
        fd = other.fd;
        other.fd = -1;
        return *this;
    }

    void unlink() {
        if (unlinkat(fd, m_filename.c_str(), 0) == -1) {
            perror(("Failed to unlink " + m_filename).c_str());
        }
    }

    File(const File &) = delete;
    File &operator=(const File &) = delete;

    bool isOpen() { return fd != -1; }

    int fd = -1;

    const std::string &filename() const { return m_filename; }

private:
    std::string m_filename;
};

static void resetPressedKeys()
{
    memset(s_pressedKeys, 0, KEY_CNT * sizeof(bool));
}

static bool handleKey(const int fd)
{
    while (true) {
        input_event iev;
        int ret = read(fd, &iev, sizeof(iev));
        if (ret != sizeof(iev)) {
            if (errno == EAGAIN) {
                return true;
            }
            if (errno != ENODEV || s_verbose) perror("Short read");
            return false;
        }
        if (iev.type == EV_SYN && iev.code == SYN_DROPPED) {
            fprintf(stderr, "Got dropped events!");
            return false;
        }
        if (iev.type != EV_KEY) {
            if (s_verbose) printf("Wrong event type %d (%d: %d) ", iev.type, iev.code, iev.value);
            // Why doesn't it work when I try continue instead here? I feel dumb
            return true;
        }
        if (s_veryVerbose) printf("Correct event type %d (%d: %d) ", iev.type, iev.code, iev.value);
        if (iev.code >= KEY_CNT) {
            printf("Invalid key %d\n", iev.code);
            return false;
        }
        s_pressedKeys[iev.code] = iev.value;
        if (s_verbose) printf("key %s has state %d\n", getKeyName(iev.code).c_str(), iev.value);
    }
    return true;
}

struct Shortcut {
    std::vector<uint16_t> keys;
    std::string command;
    bool active = false;
    bool isValid() const { return !keys.empty() && !command.empty(); }
};

std::vector<File> openKeyboards(const std::unordered_map<std::string, std::string> &keyboards)
{
    std::vector<File> files;
    for (const std::pair<const std::string, std::string> &keyboard : keyboards) {
        if (s_verbose) std::cout << keyboard.first << ": " << keyboard.second << std::endl;

        File file(keyboard.second, true);
        if (!file.isOpen()) {
            continue;
        }


        files.push_back(std::move(file));
    }
    return files;
}

static std::string getConfigPath()
{
    std::string path;

    char *rawPath = getenv("XDG_CONFIG_HOME");
    if (rawPath) {
        path = std::string(rawPath);
    }
    if (path.empty()) {
        rawPath = getenv("HOME");
        if (rawPath) {
            path = getenv("HOME");
            path += "/.config";
        }
    } else if (path.find(':') != std::string::npos) {
        std::istringstream stream(path);
        std::string testPath;
        while (std::getline(stream, testPath, ':')) {
            testPath = resolvePath(testPath);
            if (!std::filesystem::exists(testPath)) {
                continue;
            }
            if (!std::filesystem::is_directory(testPath)) {
                continue;
            }

            path = testPath;
            break;
        }
    }
    path += "/shortcut-satan.conf";
    return resolvePath(path);
}

static Shortcut parseShortcut(const std::string &line)
{
    if (line.empty()) {
        return {};
    }
    if (line[0] == '#') {
        return {};
    }
    std::string name;
    std::string value;

    size_t splitPos = line.find(':');
    if (splitPos == std::string::npos) {
        puts(("Invalid line " + line).c_str());
        return {};
    }

    Shortcut shortcut;
    shortcut.command = std_sux::trim(line.substr(splitPos + 1));
    if (shortcut.command.empty()) {
        puts(("Missing command: " + line).c_str());
        return {};
    }

    const std::string keys = std_sux::trim(line.substr(0, splitPos));
    if (keys.empty()) {
        puts(("Missing keys: " + line).c_str());
        return {};
    }

    std::istringstream stream(keys);
    std::string keyString;
    while (std::getline(stream, keyString, ' ')) {
        const int keyCode = getKeyCode(std_sux::trim(keyString));
        if (keyCode == -1) {
            puts(("Invalid key " + keyString).c_str());
            return {};
        }
        shortcut.keys.push_back(keyCode);
        s_usedKeys[keyCode] = true;
    }
    return shortcut;
}

std::vector<Shortcut> parseConfig(const std::string &path)
{
    if (!std::filesystem::exists(path)) {
        puts((path + " does not exist").c_str());
        return {};
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        perror(("Failed to open config file" + path).c_str());
        return {};
    }

    std::vector<Shortcut> ret;
    std::string line;
    while (std::getline(file, line)) {
        Shortcut shortcut = parseShortcut(std_sux::trim(line));
        if (!shortcut.isValid()) {
            continue;
        }
        ret.push_back(shortcut);
    }

    return ret;
}

void signalHandler(int sig)
{
    signal(sig, SIG_DFL);
    s_running = false;
}

int main(int argc, char *argv[])
{
    bool printKeys = false;
    for (int i=1; i<argc; i++) {
        const std::string arg(argv[i]);

        if (arg == "-p" || arg == "--printkeys") {
            printKeys = true;
            continue;
        }
        if (arg == "-v" || arg == "--verbose") {
            s_verbose = true;
            continue;
        }
        if (arg == "-vv" || arg == "--very-verbose") {
            s_verbose = true;
            continue;
        }
        if (arg == "--list-keys") {
            puts("Available keys:");
            for (const std::pair<const std::string, uint16_t> &key : key_conversion_table) {
                printf("  %s\n", key.first.c_str());
            }
            exit(0);
        }
        printf("Usage: %s [--verbose|-v|-vv|--very-verbose|--list-keys|-p|--printkeys]\n", argv[0]);
        exit(EINVAL);
    }

    File pidfile("/tmp/shortcut-satan.lock", false, O_WRONLY | O_CREAT | O_CREAT);
    if (!pidfile.isOpen() || lockf(pidfile.fd, F_TLOCK, 0) == -1) {
        if (errno == EAGAIN || errno == EACCES) {
            puts("Already running");
        } else {
            perror("Failed to create lock file");
        }
        return EALREADY; // lol sue me
    }

    termios origTermios;
    tcgetattr(STDIN_FILENO, &origTermios);

    if (printKeys) {
        termios newTermios = origTermios;
        newTermios.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
    }


    signal(SIGINT, &signalHandler);
    signal(SIGTERM, &signalHandler);
    signal(SIGQUIT, &signalHandler);

    signal(SIGHUP, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    UdevConnection udevConnection;

    const std::string configPath = getConfigPath();
    std::vector<Shortcut> shortcuts = parseConfig(configPath);
    for (const Shortcut &s : shortcuts) {
        for (const uint16_t k : s.keys) {
            if (s_verbose) printf("Keycode: %d\n", k);
        }
    }
    if (shortcuts.empty()) {
        puts(("Failed to load " + configPath).c_str());
        return ENOENT;
    }

    std::vector<File> files = openKeyboards(udevConnection.keyboardPaths);
    if (files.empty()) {
        fprintf(stderr, "Failed to open any keyboards\n");
        return ENODEV;
    }

    s_running = true;
    bool anyActiveShortcuts = false;
    puts("Running");

    fd_set fdset;
    while (s_running) {
        FD_ZERO(&fdset);
        int maxFd = 3;
        for (const File &file : files) {
            FD_SET(file.fd, &fdset);
            maxFd = std::max(maxFd, file.fd);
        }
        FD_SET(udevConnection.udevSocketFd, &fdset);
        maxFd = std::max(maxFd, udevConnection.udevSocketFd);

        timeval timeout;
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;

        const int events = select(maxFd + 1, &fdset, 0, 0, &timeout);
        if (events == -1) {
            if (s_running) {
                perror("Failed during select");
            }
            break;
        }

        if (events == 0) {
            // If there was a timeout, assume we might have missed some events and reset state
            resetPressedKeys();
            continue;
        }
        if (s_verbose) printf("Handling %d events\n", events);

        bool updated = false;
        bool needReload = false;
        for (std::vector<File>::iterator it = files.begin(); it != files.end();) {
            if (!FD_ISSET(it->fd, &fdset)) {
                it++;
                continue;
            }
            updated = true;
            if (s_verbose) printf("%s got updated\n", it->filename().c_str());

            bool removed = false;
            if (!handleKey(it->fd)) {
                if (errno == ENODEV) {
                    removed = true;
                    if (s_verbose) printf("\n%s gone, removing", it->filename().c_str());
                }
                if (s_verbose) puts("\nUnable to handle key, resetting state");
                // Reset pressed keys in case of an error
                resetPressedKeys();
            }
            if (s_verbose) puts("");

            if (removed) {
                it = files.erase(it);
            } else {
                it++;
            }
        }

        // Tiny performance improvement, if the key isn't used in any shortcut
        // don't loop through all shortcuts
        bool needToCheckShortcuts = anyActiveShortcuts;// In case there are active shortcuts they need to be updated in case they are inactive
        if (!anyActiveShortcuts && updated) {
            for (size_t i = 0; i<KEY_CNT; i++) {
                if (!s_usedKeys[i]) {
                    continue;
                }
                if (!s_pressedKeys[i]) {
                    continue;
                }

                needToCheckShortcuts = true;
                break;
            }
        }

        if (s_veryVerbose) {
            if (anyActiveShortcuts) {
                puts("Has active shortcuts");
            } else {
                puts("No active shortcuts");
            }

            if (!needToCheckShortcuts) {
                puts("Interesting keys pressed");
            } else {
                puts("No interesting keys pressed");
            }
        }
        if (needToCheckShortcuts) {
            if (s_veryVerbose) puts("Checking shortcuts");
            anyActiveShortcuts = false;
            for (Shortcut &shortcut : shortcuts) {
                bool activated = true;
                for (const uint16_t code : shortcut.keys) {
                    if (!s_pressedKeys[code]) {
                        if (s_veryVerbose) printf("%d missing\n", code);
                        activated = false;
                        break;
                    }
                }
                if (!activated) {
                    shortcut.active = false;
                    continue;
                }
                if (shortcut.active) {
                    anyActiveShortcuts = true;
                    continue;
                }
                shortcut.active = true;
                anyActiveShortcuts = true;
                if (s_verbose) printf("Activated '%s'\n", shortcut.command.c_str());
                launch(shortcut.command);
            }

            if (printKeys) {
                printf("\033[2K\r");
                for (int i=0; i<KEY_CNT; i++) {
                    if (s_pressedKeys[i]) {
                        printf("'%s' ", getKeyName(i).c_str());
                    }
                }
                fflush(stdout);
            }
        }

        if (FD_ISSET(udevConnection.udevSocketFd, &fdset)) {
            std::string updatedPath;
            const UdevConnection::UpdateResult result = udevConnection.update(&updatedPath);
            switch(result) {
            case UdevConnection::KeyboardAdded: {
                File file(updatedPath, true);
                if (!file.isOpen()) {
                    break;
                }
                if (s_verbose) printf("%s added\n", updatedPath.c_str());
                files.push_back(std::move(file));
                break;
            }
            case UdevConnection::KeyboardRemoved:
                for (std::vector<File>::iterator it = files.begin(); it != files.end(); it++) {
                    if (it->filename() == updatedPath) {
                        if (s_verbose) printf("%s removed, removing\n", it->filename().c_str());
                        // Assume there's just one? Neh.
                        it = files.erase(it);
                        break;
                    }
                }
                break;
            case UdevConnection::NoUpdate:
            default:
                break;
            }

            needReload = true;
        }

        if (needReload) {
            // Reset pressed keys if the keyboard numbers etc. change
            resetPressedKeys();
        }
    }
    puts("\nGoodbye");
    pidfile.unlink();

    if (printKeys) {
        tcsetattr(STDIN_FILENO, TCSANOW, &origTermios);
    }

    return 0;
}
