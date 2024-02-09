#pragma once
#include "buttons.h"
#include "IScreen.h"
#include "transitionData.h"
#include "StateManager/gameStateManager.h"
#include "coreEventIds.h"
#include "StateManager/gameEventData.h"

namespace screens
{
    class gameScreen : public IScreen
    {
    private:
        std::shared_ptr<gameStateManager> gameManager;
        size_t currentPlayerInfoId;
        size_t handCardsPoolId;
        std::vector<button> cardListButtons;
        button optionButtons[3];
        button popupButton;
        bool isPopupOpen = false;
        bool selectingCards = false;
        int currentCardButton;
        size_t topCardId;

        std::map<int, eventBus::delegate<transitionData>> transitionsMap = {
            {
                NAVIGATION_MAIN_MENU,
                eventBus::delegate<transitionData>{std::bind(&gameScreen::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_SETTINGS,
                eventBus::delegate<transitionData>{std::bind(&gameScreen::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_GAME,
                eventBus::delegate<transitionData>{std::bind(&gameScreen::onShow, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_GAME_OVER,
                eventBus::delegate<transitionData>{std::bind(&gameScreen::onHide, this, std::placeholders::_1)}
            },
        };
        std::map<int, eventBus::delegate<gameEventData>> gameEventsMap = {
            {
                GAME_START,
                eventBus::delegate<gameEventData>{std::bind(&gameScreen::onGameStart, this, std::placeholders::_1)}
            },
        };
        

    public:
        gameScreen(std::shared_ptr<renderer::renderer> rdr, std::shared_ptr<eventBus::eventBus> events,
                   std::shared_ptr<gameStateManager> gameManager)
            : IScreen(rdr, events), gameManager(gameManager)
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

        ~gameScreen() override
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

        void onHide(transitionData data)
        {
            hide();
        }

        void onShow(transitionData data)
        {
            show();
        }
        void onGameStart(gameEventData data)
        {
            showCurrentPlayerCards();
        }

        void show() override;
        void hide() override;
        void moveUp(input::inputData data) override;
        void moveDown(input::inputData data) override;
        void moveLeft(input::inputData data) override;
        void moveRight(input::inputData data) override;
        void accept(input::inputData data) override;
        void cancel(input::inputData data) override;

    private:
        void showCurrentPlayerCards();
        void updateTopCard();
        void updateCurrentPlayerName() const;
        void hidePopup();
        void openWarningPopup(std::string bodyText);
    };
}
