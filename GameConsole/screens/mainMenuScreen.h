#pragma once
#include "IScreen.h"
#include "EventBus/eventBus.h"

namespace screens
{
    class mainMenuScreen : public IScreen
    {
    private:
        size_t titleId;
        int currentButton = 0;
        size_t buttons[3];

    public:

        explicit mainMenuScreen(std::shared_ptr<renderer::renderer> rdr, std::shared_ptr<eventBus::eventBus> events):
            IScreen(rdr, events), titleId(0), buttons{}
        {
        }

        void show() override;
        void moveUp(input::inputData data) override;
        void moveDown(input::inputData data) override;
        void moveLeft(input::inputData data) override;
        void moveRight(input::inputData data) override;
        void deselectButton(int index) const;
        void selectButton(int index) const;
    };
}
