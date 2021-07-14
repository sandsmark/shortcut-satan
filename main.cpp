static bool s_running = false;
static bool s_verbose = false;
static bool s_veryVerbose = false;

#include "udevconnection.h"

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
}

struct File
{
    File (const std::string &filename_, const int flags = O_RDONLY | O_NONBLOCK | O_CLOEXEC) : filename(filename_) {
        fd = open(filename_.c_str(), flags);

        if (fd == -1) {
            perror(("Failed to open " + filename_).c_str());
        }
        if (s_verbose) printf("Opened %s\n", filename.c_str());
    }

    ~File() {
        if (fd != -1) {
            // Closing is very slow, for some reason, so print progress
            std::cout << "Closing " << fd << std::flush;
            close(fd);
            std::cout << "\033[2K\r";
        }
    }

    File(File&& other) : filename(std::move(other.filename)) {
        fd = other.fd;
        other.fd = -1;
    }
    void unlink() {
        if (unlinkat(fd, filename.c_str(), 0) == -1) {
            perror(("Failed to unlink " + filename).c_str());
        }
    }

    File(const File &) = delete;
    File &operator=(const File &) = delete;

    bool isOpen() { return fd != -1; }

    int fd = -1;
    const std::string filename;
};

bool s_pressedKeys[KEY_CNT];

static bool handleKey(const int fd)
{
    while (true) {
        input_event iev;
        int ret = read(fd, &iev, sizeof(iev));
        if (ret != sizeof(iev)) {
            if (errno == EAGAIN) {
                return true;
            }
            perror("Short read");
            return false;
        }
        if (iev.type == EV_SYN && iev.code == SYN_DROPPED) {
            fprintf(stderr, "Got dropped events!");
            return false;
        }
        if (iev.type != EV_KEY) {
            if (s_verbose) printf("Wrong event type %d (%d: %d)\n", iev.type, iev.code, iev.value);
            // Why doesn't it work when I try continue instead here? I feel dumb
            return true;
        }
        s_pressedKeys[iev.code] = iev.value;
        if (s_verbose) printf("Read %d: %d\n", iev.code, iev.value);
        if (s_verbose) std::cout << s_pressedKeys[KEY_MUTE] << std::endl;
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

        File file(keyboard.second);
        if (!file.isOpen()) {
            continue;
        }

        long bits[KEY_CNT] = { 0 };
        int ret = ioctl(file.fd, EVIOCGBIT(EV_KEY, sizeof(bits)), bits);
        if (ret < 0) {
            perror(("Failed to get key bits from " + keyboard.second).c_str());
            continue;
        }

        files.push_back(std::move(file));
    }
    return files;
}

static int getKeyCode(std::string input)
{
    std::transform(input.begin(), input.end(), input.begin(),
            [](char c){ return std::toupper(c); });

    std::unordered_map<std::string, uint16_t>::const_iterator it = key_conversion_table.find(input);
    if (it != key_conversion_table.end()) {
        return it->second;
    }

    return -1;
}

static std::string resolvePath(const std::string &path)
{
    if (path.empty()) {
        return {};
    }

    wordexp_t expanded;
    wordexp(path.c_str(), &expanded, 0);
    std::string resolved(expanded.we_wordv[0]);
    wordfree(&expanded);

    return resolved;
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

inline std::string trimString(std::string string)
{
    string.erase(string.begin(), std::find_if(string.begin(), string.end(), [](int c) { return !std::isspace(c); }  ));
    string.erase(std::find_if(string.rbegin(), string.rend(), [](int c) { return !std::isspace(c); }).base(), string.end());
    return string;
}

static Shortcut parseShortcut(const std::string &line)
{
    if (line.empty()) {
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
    shortcut.command = trimString(line.substr(splitPos + 1));
    if (shortcut.command.empty()) {
        puts(("Missing command: " + line).c_str());
        return {};
    }

    const std::string keys = trimString(line.substr(0, splitPos));
    if (keys.empty()) {
        puts(("Missing keys: " + line).c_str());
        return {};
    }

    std::istringstream stream(keys);
    std::string keyString;
    while (std::getline(stream, keyString, ' ')) {
        const int keyCode = getKeyCode(trimString(keyString));
        if (keyCode == -1) {
            puts(("Invalid key " + keyString).c_str());
            return {};
        }
        shortcut.keys.push_back(keyCode);
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
        Shortcut shortcut = parseShortcut(line);
        if (!shortcut.isValid()) {
            continue;
        }
        ret.push_back(parseShortcut(line));
    }

    return ret;
}

static bool s_dryRun = false;

static void launch(const std::string &command)
{
    if (s_verbose) printf(" -> Launching '%s'\n", command.c_str());

    if (s_dryRun) {
        return;
    }

    const int pid = fork();
    switch(pid) {
    case 0: {

        if (s_verbose) printf("Child executing %s\n", command.c_str());

        int maxfd = FD_SETSIZE;
        struct rlimit rl;
        if (getrlimit(RLIMIT_NOFILE, &rl) == 0) {
            maxfd = rl.rlim_cur;
        }
        for (int fd = 3; fd < maxfd; ++fd) {
            close(fd);
        }

        // SIGCHLD will not be reset after fork, unlike all other signals
        signal(SIGCHLD, SIG_DFL);

        const int ret = std::system(command.c_str());
        if (ret != 0 && s_verbose) {
            perror("Launching failed");
        }
        exit(0);
        break;
    }
    case -1:
        perror(" ! Error forking");
        return;
    default:
        if (s_verbose) printf("Forked, parent PID: %d\n", pid);
        break;
    }
}

void signalHandler(int sig)
{
    signal(sig, SIG_DFL);
    s_running = false;
}

int main(int argc, char *argv[])
{
    for (int i=1; i<argc; i++) {
        const std::string arg(argv[i]);

        if (arg == "-v" || arg == "--verbose") {
            s_verbose = true;
            continue;
        }
        if (arg == "-vv" || arg == "--very-verbose") {
            s_verbose = true;
            continue;
        }
        printf("Usage: %s [--verbose|-v|-vv|--very-verbose]\n", argv[0]);
        exit(0);
    }

    File pidfile("/tmp/shortcut-satan.lock", O_WRONLY | O_CREAT | O_CREAT);
    if (!pidfile.isOpen() || lockf(pidfile.fd, F_TLOCK, 0) == -1) {
        if (errno == EAGAIN || errno == EACCES) {
            puts("Already running");
        } else {
            perror("Failed to create lock file");
        }
        return EALREADY; // lol sue me
    }

    signal(SIGINT, &signalHandler);
    signal(SIGTERM, &signalHandler);
    signal(SIGQUIT, &signalHandler);
    signal(SIGHUP, [](int){}); // Ignore

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
            memset(s_pressedKeys, 0, KEY_CNT * sizeof(uint8_t));
            continue;
        }
        if (s_verbose) printf("Handling %d events\n", events);

        bool updated = false;
        for (const File &file : files) {
            if (!FD_ISSET(file.fd, &fdset)) {
                continue;
            }
            updated = true;
            if (s_verbose) printf("%s got updated\n", file.filename.c_str());

            if (!handleKey(file.fd)) {
                // Reset pressed keys in case of an error
                memset(s_pressedKeys, 0, KEY_CNT * sizeof(bool));
                break;
            }
        }

        if (updated) {
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
                    continue;
                }
                shortcut.active = true;
                if (s_verbose) printf("Activated %s\n", shortcut.command.c_str());
                launch(shortcut.command);
            }
        }


        if (FD_ISSET(udevConnection.udevSocketFd, &fdset)) {
            if (udevConnection.update()) {
                // Reset pressed keys if the keyboard numbers etc. change
                memset(s_pressedKeys, 0, KEY_CNT * sizeof(uint8_t));
                files = openKeyboards(udevConnection.keyboardPaths);
            }
        }
    }
    puts("\033[2K\rGoodbye");
    pidfile.unlink();

    return 0;
}
