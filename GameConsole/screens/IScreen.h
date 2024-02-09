#pragma once
#include "../renderer/renderer.h"
#include "EventBus/eventBus.h"
#include "../eventIds.h"
#include "../input/inputData.h"


namespace screens
{
    class IScreen
    {
    protected:
        std::shared_ptr<renderer::renderer> rdr;
        std::shared_ptr<eventBus::eventBus> events;
        std::map<int, eventBus::delegate<input::inputData>> eventsMap = {
            {
                INPUT_UP,
                eventBus::delegate<input::inputData>{std::bind(&IScreen::moveUp, this, std::placeholders::_1)}
            },
            {
                INPUT_DOWN,
                eventBus::delegate<input::inputData>{std::bind(&IScreen::moveDown, this, std::placeholders::_1)}
            },
            {
                INPUT_LEFT,
                eventBus::delegate<input::inputData>{std::bind(&IScreen::moveLeft, this, std::placeholders::_1)}
            },
            {
                INPUT_RIGHT,
                eventBus::delegate<input::inputData>{std::bind(&IScreen::moveRight, this, std::placeholders::_1)}
            },
            {
                INPUT_OK,
                eventBus::delegate<input::inputData>{std::bind(&IScreen::accept, this, std::placeholders::_1)}
            },
            {
                INPUT_CANCEL,
                eventBus::delegate<input::inputData>{std::bind(&IScreen::cancel, this, std::placeholders::_1)}
            },
        };
        bool hidden = true;
        bool blockInputs = true;

        virtual void subscribeToEvents()
        {
            for (std::pair<const int, eventBus::delegate<input::inputData>> eventMap : eventsMap)
            {
                size_t id = events->subscribe<input::inputData>(eventMap.first, eventMap.second.action);
                eventsMap[eventMap.first].uid = id;
            }
        }

        virtual void unSubscribeToEvents()
        {
            for (std::pair<const int, eventBus::delegate<input::inputData>> event_map : eventsMap)
            {
                events->unsubscribe<input::inputData>(event_map.first, event_map.second);
            }
        }

    public:
        IScreen(std::shared_ptr<renderer::renderer>& rdr,
                std::shared_ptr<eventBus::eventBus>& events) : rdr(rdr), events(events)
        {
        }

        virtual ~IScreen()
        {
        }

        virtual void show()
        {
            subscribeToEvents();
            blockInputs = false;
            hidden = false;
        }

        virtual void hide()
        {
            unSubscribeToEvents();
            blockInputs = true;
            hidden = true;
        }

        virtual void moveUp(input::inputData data) = 0;
        virtual void moveDown(input::inputData data) = 0;
        virtual void moveLeft(input::inputData data) = 0;
        virtual void moveRight(input::inputData data) = 0;
        virtual void accept(input::inputData data) = 0;
        virtual void cancel(input::inputData data) = 0;
    };
}
