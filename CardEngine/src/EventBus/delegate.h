#pragma once
#include <functional>

namespace eventBus
{
    template <typename T>
    struct delegate
    {
        delegate() = default;
        delegate(size_t uid, std::function<void(T)> action) : uid(uid), action(action)
        {
        }

        delegate(std::function<void(T)> action) : delegate(0, action)
        {
        }

        size_t uid;
        std::function<void(T)> action;
    };
}
