#pragma once
#include <functional>
#include <vector>

#include "abstractEventBase.h"

namespace eventBus
{
    template <typename T>
    class event : public abstractEventBase
    {
    public:
        using FunctionType = std::function<void(T)>;

        void subscribe(const FunctionType& function)
        {
            subscribers.push_back(function);
        }

        void fire(T data) const
        {
            for (const FunctionType& subscriber : subscribers)
            {
                subscriber(data);
            }
        }

    private:
        std::vector<FunctionType> subscribers;
    };
}
