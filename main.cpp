#include "udevconnection.h"

#include <iostream>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    UdevConnection conn;

    for (const std::pair<const std::string, std::string> &keyboard : conn.keyboardPaths) {
        std::cout << keyboard.first << ": " << keyboard.second << std::endl;
    }

    return 0;
}
