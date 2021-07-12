#include "udevconnection.h"

#include "keys.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>


extern "C" {
#include <linux/input.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wordexp.h>
}

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define CHECK_BIT(array, bit)	((array[LONG(bit)] >> OFF(bit)) & 1)

struct File
{
    File (const std::string &filename) {
        fd = open(filename.c_str(), O_RDONLY | O_NONBLOCK);

        if (fd == -1) {
            perror(("Failed to open " + filename).c_str());
        }
    }

    ~File() {
        if (fd != -1) {
            close(fd);
        }
    }

    File(File&& other) {
        fd = other.fd;
        other.fd = -1;
    }

    File(const File &) = delete;
    File &operator=(const File &) = delete;

    bool isOpen() { return fd != -1; }

    int fd = -1;
};

static bool s_running = false;
static bool s_verbose = true;
static bool s_veryVerbose = false;

bool s_pressedKeys[KEY_CNT];

static bool handleKey(const int fd)
{
    input_event iev;
    int ret = read(fd, &iev, sizeof(iev));
    if (ret != sizeof(iev)) {
        perror("read");
        return false;
    }
    if (iev.type != EV_KEY) {
        return true;
    }
    s_pressedKeys[iev.code] = iev.value;
    if (s_verbose) printf("Read %d: %d\n", iev.code, iev.value);
    if (s_verbose) std::cout << s_pressedKeys[KEY_MUTE] << std::endl;
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
        std::cout << keyboard.first << ": " << keyboard.second << std::endl;

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

        // Just some sanity checking
        if (!(CHECK_BIT(bits, KEY_ESC))) {
            puts((keyboard.second + " missing escape key").c_str());
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
    std::cout << " -> Launching '" << command << "'" << std::endl;

    if (s_dryRun) {
        return;
    }

    const int pid = fork();
    switch(pid) {
    case 0: {
        if (s_verbose) std::cout << "Child executing " << command << std::endl;

        const int ret = std::system(command.c_str());
        if (ret != 0 && s_verbose) {
            perror("Launching failed");
        }
        exit(0);
        break;
    }
    case -1:
        std::cerr << " ! Error forking: " << strerror(errno) << std::endl;
        return;
    default:
        if (s_verbose) std::cout << "Forked, parent PID: " << pid << std::endl;
        break;
    }
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
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
    //std::vector<std::string> test = {"left", "ctrl"};

#if 0
    Shortcut shrt;
    for (const std::string &key : test) {
        const int keycode = getKeyCode(key);
        if (keycode == -1) {
            puts(("Failed to find " + key).c_str());
            continue;
        }
        shrt.keys.push_back(keycode);
    }
    shortcuts.push_back(std::move(shrt));
#endif

    std::vector<File> files = openKeyboards(udevConnection.keyboardPaths);
    puts("Opened");

    s_running = true;

    fd_set fdset;

    while (s_running) {
        FD_ZERO(&fdset);
        for (const File &file : files) {
            FD_SET(file.fd, &fdset);
        }
        FD_SET(udevConnection.udevSocketFd, &fdset);

        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        const int events = select(files.back().fd + 1, &fdset, 0, 0, &timeout);
        if (events == -1) {
            perror("Failed during select");
            break;
        }

        if (events == 0) { // Just a timeout
            continue;
        }

        bool updated = false;
        for (const File &file : files) {
            if (!FD_ISSET(file.fd, &fdset)) {
                continue;
            }
            updated = true;

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

    return 0;
}
