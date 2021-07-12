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
        //const char *sysName = udev_device_get_sysname(dev);
        const char *sysName = udev_device_get_property_value(dev, "DEVNAME");
        if (sysName) {
            linkPath = sysName;
        } else {
            puts("Falling back");
            udev_list_entry *devLink= udev_device_get_devlinks_list_entry(dev);
            if (!devLink) {
                return {};
            }
            const char *rawLinkPath = udev_list_entry_get_name(devLink);
            if (!rawLinkPath) {
                perror("Failed to get raw link path");
                return {};
            }
        }
        return linkPath;
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

            const char *isKeyboard = udev_device_get_property_value(dev, "ID_INPUT_KEYBOARD");
            const char *isKey = udev_device_get_property_value(dev, "ID_INPUT_KEY");
            const char *isSwitch = udev_device_get_property_value(dev, "ID_INPUT_SWITCH");

            if (!(isKeyboard && strcmp(isKeyboard, "1") == 0) && !(isKey && strcmp(isKey, "1") == 0) && !(isSwitch && strcmp(isSwitch, "1") == 0)) {
                fprintf(stderr, "!!!!!!!! Skipping non-keyboard %s\n", path);
                //printProperties(dev);
                udev_device_unref(dev);
                fprintf(stderr, " -------------\n");
                continue;
            }

            // It's a list entry, but we only need one
            std::string linkPath = devicePath(dev);
            if (linkPath.empty() || !std::filesystem::exists(linkPath)) {
                fprintf(stderr, "Skipping device not in /dev: %s (%s)\n", path, linkPath.c_str());
                //printProperties(dev);
                udev_device_unref(dev);
                continue;
            }

            fprintf(stdout, "Found keyboard: %s: %s\n", path, linkPath.c_str());
            printProperties(dev);
            const std::string id = udev_device_get_devpath(dev);
            keyboardPaths[id] = linkPath;

            udev_device_unref(dev);
        }

        udev_enumerate_unref(enumerate);
        printf("Got %ld keyboards\n", keyboardPaths.size());
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
        //fprintf(stderr, "action: %s\n", udev_device_get_action(dev));
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
        if (action == "remove" || action == "offline") {
            keyboardPaths.erase(id);
            return true;
        }

        // Not initialized yet
        if (!udev_device_get_is_initialized(dev)) {
            return false;
        }

        if (keyboardPaths.count(id) != 0) {
            return false;
        }
        udev_list_entry *devLinkPath = udev_device_get_devlinks_list_entry(dev);
        if (!devLinkPath) {
            fprintf(stderr, "Skipping device not in /dev\n");
            printProperties(dev);
            return false;
        }
        const std::string path = udev_list_entry_get_name(devLinkPath);
        keyboardPaths[id] = path;

        return true;
    }

    udev *context = nullptr;
    udev_monitor *udevMonitor = nullptr;

    bool udevAvailable = false;

    int udevSocketFd = -1;

    std::unordered_map<std::string, std::string> keyboardPaths;
};

