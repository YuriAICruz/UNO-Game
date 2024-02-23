﻿#pragma once
#include "buttons.h"
#include "editBox.h"
#include "IScreen.h"
#include "popupWindow.h"
#include "transitionData.h"
#include "client/client.h"

namespace screens
{
    class roomCreationScreen : public screens::IScreen
    {
    private:
        size_t titleId;
        int currentButton = 0;
        button buttons[2];
        popupWindow popup;
        editBox box;
        std::shared_ptr<netcode::client> netClient;

        std::map<int, eventBus::delegate<transitionData>> transitionsMap = {
            {
                NAVIGATION_MAIN_MENU,
                eventBus::delegate<transitionData>{
                    std::bind(&roomCreationScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_SETTINGS,
                eventBus::delegate<transitionData>{
                    std::bind(&roomCreationScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_GAME,
                eventBus::delegate<transitionData>{
                    std::bind(&roomCreationScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_GAME_OVER,
                eventBus::delegate<transitionData>{
                    std::bind(&roomCreationScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_NETWORK_CONNECT,
                eventBus::delegate<transitionData>{
                    std::bind(&roomCreationScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_NETWORK_ROOMS,
                eventBus::delegate<transitionData>{
                    std::bind(&roomCreationScreen::onShow, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_NETWORK_WAIT_ROOM,
                eventBus::delegate<transitionData>{
                    std::bind(&roomCreationScreen::onHide, this, std::placeholders::_1)
                }
            },
        };

    public:
        roomCreationScreen(std::shared_ptr<renderer::renderer> rdr, std::shared_ptr<eventBus::eventBus> events,
            std::shared_ptr<netcode::client> cl)
            : IScreen(rdr, events), netClient(cl), popup(popupWindow{rdr.get()}), box(editBox(rdr.get()))
        {
            for (std::pair<const int, eventBus::delegate<transitionData>> transitionMap : transitionsMap)
            {
                size_t id = events->subscribe<transitionData>(transitionMap.first, transitionMap.second.action);
                transitionsMap[transitionMap.first].uid = id;
            }
        }

        ~roomCreationScreen() override
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
        void returnToMainScreen();

    private:
        void createNewRoom();
        void listRooms();
        void moveToNextRoom();
        void clearRoomsList();
        void selectButton(int index) const;
        void deselectButton(int index) const;
        void setButtonsSelectionColor();
    };
}
