#pragma once
#include <functional>

namespace screens
{
    struct button
    {
        size_t id;
        std::function<void()> action;
        std::function<void()> actionLeft;
        std::function<void()> actionRight;
    };
}