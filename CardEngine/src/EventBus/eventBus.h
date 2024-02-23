#pragma once
#include <map>
#include <memory>

#include "alreadyExistException.h"
#include "../../framework.h"
#include "event.h"

namespace eventBus
{
    class ENGINE_API eventBus
    {
    private:
        std::map<int, std::shared_ptr<abstractEventBase>> events;

    public:
        template <typename T>
        void bindEvent(int id)
        {
            if (!events.count(id))
            {
                events.emplace(id, std::make_shared<event<T>>());
                return;
            }

            throw new alreadyExistException();
        }

        template <typename T>
        size_t subscribe(int id, std::function<void(T)> func)
        {
            return static_cast<event<T>*>(events.at(id).get())->subscribe(func);
        }
        template <typename T>
        void unsubscribe(int id, delegate<T> function)
        {
            static_cast<event<T>*>(events.at(id).get())->unsubscribe(function);
        }

        template <typename T>
        void clear(int id)
        {
            static_cast<event<T>*>(events.at(id).get())->clear();
        }

        template <typename T>
        void fireEvent(int id, T data)
        {
            static_cast<event<T>*>(events.at(id).get())->fire(data);
        }
    };
}
