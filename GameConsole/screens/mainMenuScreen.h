#pragma once
#include "buttons.h"
#include "IScreen.h"
#include "transitionData.h"
#include "EventBus/eventBus.h"

namespace screens
{
    class mainMenuScreen : public IScreen
    {
    private:
        size_t titleId;
        int currentButton = 0;
        button buttons[4];
        std::map<int, eventBus::delegate<transitionData>> transitionsMap = {
            {
                NAVIGATION_MAIN_MENU,
                eventBus::delegate<transitionData>{std::bind(&mainMenuScreen::onShow, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_SETTINGS,
                eventBus::delegate<transitionData>{std::bind(&mainMenuScreen::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_GAME,
                eventBus::delegate<transitionData>{std::bind(&mainMenuScreen::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_GAME_OVER,
                eventBus::delegate<transitionData>{std::bind(&mainMenuScreen::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_NETWORK_CONNECT,
                eventBus::delegate<transitionData>{std::bind(&mainMenuScreen::onHide, this, std::placeholders::_1)}
            },
        };

    public:
        explicit mainMenuScreen(std::shared_ptr<renderer::renderer> rdr, std::shared_ptr<eventBus::eventBus> events):
            IScreen(rdr, events), titleId(0), buttons{}
        {
            for (std::pair<const int, eventBus::delegate<transitionData>> transitionMap : transitionsMap)
            {
                size_t id = events->subscribe<transitionData>(transitionMap.first, transitionMap.second.action);
                transitionsMap[transitionMap.first].uid = id;
            }
        }

        ~mainMenuScreen() override
        {
            for (std::pair<const int, eventBus::delegate<transitionData>> transitionMap : transitionsMap)
            {
                events->unsubscribe<transitionData>(transitionMap.first, transitionMap.second);
            }
        }

        void onHide(transitionData data)
        {
            hide();
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
        void selectButton(int index) const;
        void deselectButton(int index) const;
    };
}
