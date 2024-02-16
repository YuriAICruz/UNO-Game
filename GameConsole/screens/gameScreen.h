#pragma once
#include "buttons.h"
#include "IScreen.h"
#include "transitionData.h"
#include "StateManager/gameStateManager.h"
#include "coreEventIds.h"
#include "popupWindow.h"
#include "StateManager/gameEventData.h"
#include "../renderer/elements/card.h"

namespace screens
{
    class gameScreen : public IScreen
    {
    private:
        gameStateManager* gameManager;
        size_t currentPlayerInfoId;
        size_t handCardsPoolId;
        std::vector<button> cardListButtons;
        button optionButtons[3];
        popupWindow popup;
        bool selectingCards = false;
        bool selectingOptions = false;
        int currentCardButton = 0;
        int currentOptionButton = 0;
        button unoYell[2];        
        size_t topCardId;
        int cardSizeX = 8;
        int cardSizeY = 6;
        bool unoPopup = false;
        bool cardsAreHidden = false;
        int isOnline = false;

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
                NAVIGATION_GAME_OVER,
                eventBus::delegate<transitionData>{std::bind(&gameScreen::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_NETWORK_CONNECT,
                eventBus::delegate<transitionData>{std::bind(&gameScreen::onHide, this, std::placeholders::_1)}
            },
        };
        std::map<int, eventBus::delegate<gameEventData>> gameEventsMap = {
            {
                GAME_START,
                eventBus::delegate<gameEventData>{std::bind(&gameScreen::onGameStart, this, std::placeholders::_1)}
            },
            {
                GAME_NO_UNO_PENALTY,
                eventBus::delegate<gameEventData>{std::bind(&gameScreen::onUnoPenalty, this, std::placeholders::_1)}
            },
            {
                GAME_END,
                eventBus::delegate<gameEventData>{std::bind(&gameScreen::onGameEnded, this, std::placeholders::_1)}
            },
        };

    public:
        gameScreen(std::shared_ptr<renderer::renderer> rdr, std::shared_ptr<eventBus::eventBus> events)
            : IScreen(rdr, events), popup(popupWindow(rdr.get()))
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


        void show() override;
        void hide() override;
        void setGameManager(gameStateManager* gm);
        void moveUp(input::inputData data) override;
        void moveDown(input::inputData data) override;
        void moveLeft(input::inputData data) override;
        void moveRight(input::inputData data) override;
        void accept(input::inputData data) override;
        void cancel(input::inputData data) override;
        void hideUnoPopup();
        void showUnoPopup();

    private:
        void onShow(transitionData data)
        {
            show();
        }
        void onGameStart(gameEventData data)
        {
            switchToCards();
            showCurrentPlayerCards(true);
        }
        void onUnoPenalty(gameEventData data);
        void onGameEnded(gameEventData data);
        
        void tryYellUno();
        void tryToPass();
        void tryDrawMoreCards();
        void expandCardsPool(int hand_size);
        void hideCard(std::vector<button>::const_reference button);
        void drawOneCard();
        void showCurrentPlayerCards(bool hidden);
        void setCardData(elements::card* cardElement, cards::ICard* card, bool hidden);
        void updateTopCard();
        void updateCurrentPlayerName() const;
        void selectCard(int index);
        void switchToCards();
        void switchToOptions();
        void selectCardButton(int index) const;
        void deselectCardButton(int index) const;
        void selectOptionButton(int index) const;
        void deselectOptionButton(int index) const;
    };
}
