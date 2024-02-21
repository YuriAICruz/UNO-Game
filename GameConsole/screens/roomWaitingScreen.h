#pragma once
#include "buttons.h"
#include "editBox.h"
#include "IScreen.h"
#include "netGameStateManager.h"
#include "popupWindow.h"
#include "transitionData.h"
#include "client/client.h"

namespace screens
{
    class roomWaitingScreen : public IScreen
    {
    private:
        int titleId;
        int currentButton = 0;
        int handSize = 7;
        button buttons[4];
        popupWindow popup;
        editBox box;
        std::shared_ptr<netcode::client> netClient;
        netGameStateManager* netGameManager;
        std::string cardsPath;
        size_t seed;

        std::map<int, eventBus::delegate<transitionData>> transitionsMap = {
            {
                NAVIGATION_MAIN_MENU,
                eventBus::delegate<transitionData>{
                    std::bind(&roomWaitingScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_SETTINGS,
                eventBus::delegate<transitionData>{
                    std::bind(&roomWaitingScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_GAME,
                eventBus::delegate<transitionData>{
                    std::bind(&roomWaitingScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_GAME_OVER,
                eventBus::delegate<transitionData>{
                    std::bind(&roomWaitingScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_NETWORK_CONNECT,
                eventBus::delegate<transitionData>{
                    std::bind(&roomWaitingScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_NETWORK_ROOMS,
                eventBus::delegate<transitionData>{
                    std::bind(&roomWaitingScreen::onHide, this, std::placeholders::_1)
                }
            },
        };

    public:
        roomWaitingScreen(std::shared_ptr<renderer::renderer> rdr, std::shared_ptr<eventBus::eventBus> events,
                          std::shared_ptr<netcode::client> cl
        )
            : IScreen(rdr, events), netClient(cl), popup(popupWindow(rdr.get())), box(editBox(rdr.get()))
        {
            for (std::pair<const int, eventBus::delegate<transitionData>> transitionMap : transitionsMap)
            {
                size_t id = events->subscribe<transitionData>(transitionMap.first, transitionMap.second.action);
                transitionsMap[transitionMap.first].uid = id;
            }
        }

        ~roomWaitingScreen() override
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
        void hide() override;
        void moveUp(input::inputData data) override;
        void moveDown(input::inputData data) override;
        void moveLeft(input::inputData data) override;
        void moveRight(input::inputData data) override;
        void accept(input::inputData data) override;
        void cancel(input::inputData data) override;
        void setGameManager(netGameStateManager* gameManager);
        void setGameSettings(std::string cardsPath, size_t seed);

    private:
        void updateStartButton(netcode::room* room);
        void tryExitRoom();
        void exitRoomAndReturnToMainScreen();
        void startRoomGame();
        void goToGameScreen();
        void updateStatingCardsCount();
        void decreaseStartingCards();
        void increaseStartingCards();
        void updatePlayerName() const;
        void setPlayerName();
        void selectButton(int index) const;
        void deselectButton(int index) const;
    };
}
