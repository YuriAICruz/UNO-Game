#pragma once
#include "IScreen.h"
#include "client/client.h"
#include "transitionData.h"
#include "buttons.h"
#include "EventBus/eventBus.h"

namespace screens
{
    class connectToServerScreen : public IScreen
    {
    private:
        size_t titleId;
        int currentButton = 0;
        button buttons[3];
        std::shared_ptr<netcode::client> netClient;
        std::string serverAddr = "tcp://127.0.0.1:8080";

        std::map<int, eventBus::delegate<transitionData>> transitionsMap = {
            {
                NAVIGATION_MAIN_MENU,
                eventBus::delegate<transitionData>{
                    std::bind(&connectToServerScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_SETTINGS,
                eventBus::delegate<transitionData>{
                    std::bind(&connectToServerScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_GAME,
                eventBus::delegate<transitionData>{
                    std::bind(&connectToServerScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_GAME_OVER,
                eventBus::delegate<transitionData>{
                    std::bind(&connectToServerScreen::onHide, this, std::placeholders::_1)
                }
            },
            {
                NAVIGATION_NETWORK_CONNECT,
                eventBus::delegate<transitionData>{
                    std::bind(&connectToServerScreen::onShow, this, std::placeholders::_1)
                }
            },
        };

    public:
        explicit connectToServerScreen(
            std::shared_ptr<renderer::renderer> rdr,
            std::shared_ptr<eventBus::eventBus> events,
            std::shared_ptr<netcode::client> cl
        ):
            IScreen(rdr, events),
            netClient(cl)
        {
            for (std::pair<const int, eventBus::delegate<transitionData>> transitionMap : transitionsMap)
            {
                size_t id = events->subscribe<transitionData>(transitionMap.first, transitionMap.second.action);
                transitionsMap[transitionMap.first].uid = id;
            }
        }

        ~connectToServerScreen() override
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

    private:
        void updateServerAddress();
    };
}
