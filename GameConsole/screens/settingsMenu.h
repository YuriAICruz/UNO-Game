﻿#pragma once
#include "buttons.h"
#include "IScreen.h"
#include "transitionData.h"

namespace screens
{
    class settingsMenu : public IScreen
    {
    private:
        button buttons[1];
        int currentButton = 0;
        std::map<int, eventBus::delegate<transitionData>> transitionsMap = {
            {
                NAVIGATION_MAIN_MENU,
                eventBus::delegate<transitionData>{std::bind(&settingsMenu::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_SETTINGS,
                eventBus::delegate<transitionData>{std::bind(&settingsMenu::onShow, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_GAME,
                eventBus::delegate<transitionData>{std::bind(&settingsMenu::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_GAME_OVER,
                eventBus::delegate<transitionData>{std::bind(&settingsMenu::onHide, this, std::placeholders::_1)}
            },
        };

    public:
        settingsMenu(std::shared_ptr<renderer::renderer> rdr, std::shared_ptr<eventBus::eventBus> events)
            : IScreen(rdr, events)
        {
            for (std::pair<const int, eventBus::delegate<transitionData>> transitionMap : transitionsMap)
            {
                size_t id = events->subscribe<transitionData>(transitionMap.first, transitionMap.second.action);
                transitionsMap[transitionMap.first].uid = id;
            }
        }

        ~settingsMenu() override
        {
            for (std::pair<const int, eventBus::delegate<transitionData>> transitionMap : transitionsMap)
            {
                events->unsubscribe<transitionData>(transitionMap.first, transitionMap.second);   
            }
        }

        void onShow(transitionData data)
        {
            show();
        }

        void onHide(transitionData data)
        {
            hide();
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