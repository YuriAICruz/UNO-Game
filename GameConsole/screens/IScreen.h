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

    public:
        virtual ~IScreen() = default;

        IScreen(std::shared_ptr<renderer::renderer>& rdr, std::shared_ptr<eventBus::eventBus>& events) : rdr(rdr), events(events)
        {
            events->subscribe<input::inputData>(INPUT_UP, std::bind(&IScreen::moveUp, this, std::placeholders::_1));
            events->subscribe<input::inputData>(INPUT_DOWN, std::bind(&IScreen::moveDown, this, std::placeholders::_1));
            events->subscribe<input::inputData>(INPUT_LEFT, std::bind(&IScreen::moveLeft, this, std::placeholders::_1));
            events->subscribe<input::inputData>(INPUT_RIGHT, std::bind(&IScreen::moveRight, this, std::placeholders::_1));
        }

        virtual void moveUp(input::inputData data) = 0;
        virtual void moveDown(input::inputData data) = 0;
        virtual void moveLeft(input::inputData data) = 0;
        virtual void moveRight(input::inputData data) = 0;

        virtual void show() = 0;
    };
}
