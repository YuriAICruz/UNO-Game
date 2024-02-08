#pragma once
#include "IScreen.h"
#include "transitionData.h"
#include "EventBus/eventBus.h"

namespace screens
{
    class mainMenuScreen : public IScreen
    {
    private:
        struct button
        {
            size_t id;
            std::function<void()> action;
        };

        size_t titleId;
        int currentButton = 0;
        button buttons[3];

    public:
        explicit mainMenuScreen(std::shared_ptr<renderer::renderer> rdr, std::shared_ptr<eventBus::eventBus> events):
            IScreen(rdr, events), titleId(0), buttons{}
        {
            events->subscribe<transitionData>(
                NAVIGATION_MAIN_MENU,
                std::bind(&mainMenuScreen::onShow, this, std::placeholders::_1)
            );
        }

        void onShow(transitionData data)
        {
            show();
        }

        void show() override;
        void moveUp(input::inputData data) override;
        void moveDown(input::inputData data) override;
        void moveLeft(input::inputData data) override;
        void moveRight(input::inputData data) override;
        void accept(input::inputData data) override;
        void cancel(input::inputData data) override;
        void deselectButton(int index) const;
        void selectButton(int index) const;
    };
}
