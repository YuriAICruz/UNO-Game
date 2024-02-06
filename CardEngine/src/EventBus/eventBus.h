#pragma once
#include <map>
#include <memory>

#include "alreadyExistException.h"
#include "event.h"

namespace eventBus
{
    struct abstractEventBase;

    class eventBus
    {
    private:
        std::map<int, std::unique_ptr<abstractEventBase>> events;

    public:
        template <typename T>
        void bindEvent(int id)
        {
            if (!events.count(id))
            {
                events.emplace(id, std::make_unique<event<T>>());
                return;
            }

            throw new alreadyExistException();
        }

        template <typename T>
        void subscribe(int id, std::function<void(T)> func)
        {
            static_cast<event<T>*>(events.at(id).get())->subscribe(func);
        }

        template <typename T>
        void fireEvent(int id, T data)
        {
            static_cast<event<T>*>(events.at(id).get())->fire(data);
        }
    };
}
