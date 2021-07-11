#include "udevconnection.h"

#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

extern "C" {
#include <linux/input.h>
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
        printf("Opened %s: %d\n", filename.c_str(), fd);
    }

    ~File() {
        if (fd != -1) {
            printf("Closed %d\n", fd);
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

static bool handleKey(const int fd)
{
    input_event iev;
    int ret = read(fd, &iev, sizeof(iev));
    if (ret != sizeof(iev)) {
        perror("read");
        return false;
    }
    printf("Read %d\n", iev.type);
    return true;
}

std::vector<File> openKeyboards(const std::vector<std::pair<std::string, std::string>> &keyboards)
{
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    UdevConnection udevConnection;

    std::vector<File> files;
    for (const std::pair<const std::string, std::string> &keyboard : udevConnection.keyboardPaths) {
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
        for (const File &file : files) {
            if (!FD_ISSET(file.fd, &fdset)) {
                continue;
            }
            handleKey(file.fd);
        }

        if (FD_ISSET(udevConnection.udevSocketFd, &fdset)) {
            udevConnection.update();
        }
        printf("Select complete, ret: %d\n", events);
    }

    return 0;
}
