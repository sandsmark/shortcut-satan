#pragma once

#include <functional>

struct OnReturn {
    OnReturn(const std::function<void()> &func) : onDestroy(func) {}
    ~OnReturn() { onDestroy(); }

    const std::function<void()> onDestroy;
};

