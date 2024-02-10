#pragma once
#include "coreEventIds.h"
#include "IScreen.h"
#include "transitionData.h"
#include "StateManager/gameEventData.h"

namespace screens
{
    class gameOverScreen : public screens::IScreen
    {
    private:
        int returnCount;
        turnSystem::IPlayer* winnerPlayer = nullptr;
        size_t winnerPlayerNameId;

        std::map<int, eventBus::delegate<transitionData>> transitionsMap = {
            {
                NAVIGATION_MAIN_MENU,
                eventBus::delegate<transitionData>{std::bind(&gameOverScreen::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_SETTINGS,
                eventBus::delegate<transitionData>{std::bind(&gameOverScreen::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_GAME,
                eventBus::delegate<transitionData>{std::bind(&gameOverScreen::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_GAME_OVER,
                eventBus::delegate<transitionData>{std::bind(&gameOverScreen::onShow, this, std::placeholders::_1)}
            },
        };
        std::map<int, eventBus::delegate<gameEventData>> gameEventsMap = {
            {
                GAME_END,
                eventBus::delegate<gameEventData>{std::bind(&gameOverScreen::onGameEnded, this, std::placeholders::_1)}
            },
        };

    public:
        gameOverScreen(std::shared_ptr<renderer::renderer> rdr, std::shared_ptr<eventBus::eventBus> events)
            : IScreen(rdr, events)
        {
            for (std::pair<const int, eventBus::delegate<transitionData>> transitionMap : transitionsMap)
            {
                size_t id = events->subscribe<transitionData>(transitionMap.first, transitionMap.second.action);
                transitionsMap[transitionMap.first].uid = id;
            }
            for (std::pair<const int, eventBus::delegate<gameEventData>> gameEventMap : gameEventsMap)
            {
                size_t id = events->subscribe<gameEventData>(gameEventMap.first, gameEventMap.second.action);
                gameEventsMap[gameEventMap.first].uid = id;
            }
        }

        ~gameOverScreen() override
        {
            for (std::pair<const int, eventBus::delegate<transitionData>> transitionMap : transitionsMap)
            {
                events->unsubscribe<transitionData>(transitionMap.first, transitionMap.second);
            }
            for (std::pair<const int, eventBus::delegate<gameEventData>> gameEventMap : gameEventsMap)
            {
                events->unsubscribe<gameEventData>(gameEventMap.first, gameEventMap.second);
            }
        }


        void show() override;
        void moveUp(input::inputData data) override;
        void moveDown(input::inputData data) override;
        void moveLeft(input::inputData data) override;
        void moveRight(input::inputData data) override;
        void accept(input::inputData data) override;
        void cancel(input::inputData data) override;
    private:

        void onHide(transitionData data)
        {
            hide();
        }

        void onShow(transitionData data)
        {
            show();
        }

        void onGameEnded(gameEventData data)
        {
            winnerPlayer = data.player;
            updateWinningPlayerName();
        }
        void updateWinningPlayerName();
    };
}
