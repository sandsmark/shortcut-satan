#pragma once

extern "C" {
#include <linux/input.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <unistd.h>
#include <wordexp.h>
#include <signal.h>
}

namespace std_sux
{
    // I blame boost, but for some reason the std::string constructor doesn't like null pointers.
    // Because a convenient API, like QString, would be too much to ask for.
    std::string string(const char *str) { return std::string(str ? str : ""); }

    // The boost style APIs in std really hate convenience
    inline std::string trim(std::string string)
    {
        string.erase(string.begin(), std::find_if(string.begin(), string.end(), [](int c) { return !std::isspace(c); }  ));
        string.erase(std::find_if(string.rbegin(), string.rend(), [](int c) { return !std::isspace(c); }).base(), string.end());
        return string;
    }

    inline std::string uppercase(std::string input) {
        std::transform(input.begin(), input.end(), input.begin(),
                [](char c){ return std::toupper(c); });
        return input;
    }

}// namespace std_sux

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

        // We use system(), because this is basically what system() is for and
        // we have trusted data.
        // Trying to do the same as system() ourselves (like hkd with execvp)
        // is probably going to break, with manually resolving commands etc.
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
