#pragma once

#include <string>
#include <algorithm>
#include <filesystem>

extern "C" {
#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
}

#include "onreturn.h"
#include "utils.h"

struct UdevConnection {
    UdevConnection()
    {
        context = udev_new();

        if (!context) {
            fprintf(stderr, "Failed to connect to udev\n");
            return;
        }

        udevMonitor = udev_monitor_new_from_netlink(context, "udev");

        if (!udevMonitor) {
            fprintf(stderr, "Failed to create udev monitor\n");
            return;
        }

        udev_monitor_filter_add_match_subsystem_devtype(udevMonitor, "input", 0);
        udev_monitor_enable_receiving(udevMonitor);
        udevSocketFd = udev_monitor_get_fd(udevMonitor);
        udevAvailable = true;

        init();
    }

    static std::string devicePath(udev_device *dev)
    {
        std::string linkPath;
        const std::string sysName = std_sux::string(udev_device_get_property_value(dev, "DEVNAME"));
        if (!sysName.empty()) {
            linkPath = sysName;
        } else {
            if (s_verbose) puts("Falling back");
            udev_list_entry *devLink= udev_device_get_devlinks_list_entry(dev);
            if (!devLink) {
                return {};
            }
            linkPath = std_sux::string(udev_list_entry_get_name(devLink));
        }
        return linkPath;
    }
    void addKeyboard(udev_device *dev)
    {
        const std::string id = udev_device_get_devpath(dev);

        const std::string isKeyboard = std_sux::string(udev_device_get_property_value(dev, "ID_INPUT_KEYBOARD"));
        const std::string isKey = std_sux::string(udev_device_get_property_value(dev, "ID_INPUT_KEY"));

        if (isKeyboard != "1" && isKey != "1") {
            if (s_verbose) fprintf(stderr, "!!!!!!!! Skipping non-keyboard %s\n", id.c_str());
            if (s_veryVerbose) printProperties(dev);
            if (s_verbose) fprintf(stderr, " -------------\n");
            return;
        }

        // It's a list entry, but we only need one
        std::string linkPath = devicePath(dev);
        if (linkPath.empty() || !std::filesystem::exists(linkPath)) {
            if (s_verbose) fprintf(stderr, "Skipping device not in /dev: %s (%s)\n", id.c_str(), linkPath.c_str());
            if (s_veryVerbose) printProperties(dev);
            return;
        }

        // Not initialized yet
        if (!udev_device_get_is_initialized(dev)) {
            if (s_verbose) printf("%s not initialized yet\n", linkPath.c_str());
            return;
        }


        if (keyboardPaths.count(id) != 0) {
            if (s_verbose) printf("%s already added\n", linkPath.c_str());
            return;
        }

        if (s_verbose) fprintf(stdout, "Found keyboard: %s: %s\n", id.c_str(), linkPath.c_str());
        if (s_veryVerbose) printProperties(dev);
        keyboardPaths[id] = linkPath;
    }

    void init()
    {
        udev_enumerate *enumerate = udev_enumerate_new(context);

        udev_enumerate_add_match_subsystem(enumerate, "input");
        udev_enumerate_scan_devices(enumerate);

        udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
        udev_list_entry *entry = nullptr;

        udev_list_entry_foreach(entry, devices) {
            const char *path = udev_list_entry_get_name(entry);

            if (!path) {
                fprintf(stderr, "Invalid device when listing\n");
                continue;
            }

            udev_device *dev = udev_device_new_from_syspath(context, path);

            if (!dev) {
                fprintf(stderr, "failed getting %s\n", path);
                continue;
            }
            addKeyboard(dev);

            udev_device_unref(dev);
        }

        udev_enumerate_unref(enumerate);
        if (s_verbose) printf("Got %ld keyboards\n", keyboardPaths.size());
    }

    ~UdevConnection()
    {
        if (udevMonitor) {
            udev_monitor_unref(udevMonitor);
        }

        if (context) {
            udev_unref(context);
        }
    }

    void printProperties(udev_device *dev)
    {
        fprintf(stderr, "sysname: %s\n", udev_device_get_sysname(dev));
        udev_list_entry *entry = udev_device_get_properties_list_entry(dev);

        while (entry) {
            const char *name = udev_list_entry_get_name(entry);
            const char *value = udev_list_entry_get_value(entry);
            fprintf(stderr, "property name: %s value %s\n", name, value);
            entry = udev_list_entry_get_next(entry);
        }
    }

    bool update()
    {
        if (!udevAvailable) {
            fprintf(stderr, "udev unavailable\n");
            return false;
        }

        udev_device *dev = udev_monitor_receive_device(udevMonitor);
        OnReturn releaseDev([&]() {
            udev_device_unref(dev);
        });

        const std::string id = udev_device_get_devpath(dev);

        const std::string action = udev_device_get_action(dev);
        if (s_verbose) printf("udev action: %s for id %s\n", action.c_str(), id.c_str());
        if (action == "remove" || action == "offline") {
            keyboardPaths.erase(id);
            return true;
        }
        addKeyboard(dev);

        return true;
    }

    udev *context = nullptr;
    udev_monitor *udevMonitor = nullptr;

    bool udevAvailable = false;

    int udevSocketFd = -1;

    std::unordered_map<std::string, std::string> keyboardPaths;
};

