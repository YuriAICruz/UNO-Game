#pragma once
#include <functional>
#include <vector>

#include "../../framework.h"
#include "abstractEventBase.h"
#include "delegate.h"
#include "../guidGenerator.h"

namespace eventBus
{
    template <typename T>
    class ENGINE_API event : public abstractEventBase
    {
    public:
        using FunctionType = std::function<void(T)>;

        size_t subscribe(const FunctionType& function)
        {
            auto id = guidGenerator::generateGUID();
            subscribers.push_back(delegate<T>{id, function});
            return id;
        }

        void unsubscribe(delegate<T> function)
        {
            if (function.uid == 0)
            {
                return;
            }
            int index = 0;
            for (int n = subscribers.size(); index < n; ++index)
            {
                if (subscribers.at(index).uid == function.uid)
                {
                    break;
                }
            }
            if (index >= subscribers.size())
            {
                return;
            }
            subscribers.erase(subscribers.begin() + index);
        }

        void clear()
        {
            subscribers.clear();
        }

        void fire(T data) const
        {
            for (const delegate<T>& subscriber : subscribers)
            {
                subscriber.action(data);
            }
        }

    private:
        std::vector<delegate<T>> subscribers;
    };
}
