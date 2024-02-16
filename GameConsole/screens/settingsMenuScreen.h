#pragma once
#include <sstream>

#include "buttons.h"
#include "IScreen.h"
#include "transitionData.h"

namespace screens
{
    class settingsMenuScreen : public IScreen
    {
    private:
        button buttons[4];
        std::vector<button> playersButtons;
        int currentButton = 0;
        int currentPlayerButton = 0;

        std::vector<std::string> players;
        std::string configFilePath = "Data\\deck_setup.json";
        int handCount = 7;
        size_t seed = 12341234;
        bool editingPlayers = false;

        std::map<int, eventBus::delegate<transitionData>> transitionsMap = {
            {
                NAVIGATION_MAIN_MENU,
                eventBus::delegate<transitionData>{std::bind(&settingsMenuScreen::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_SETTINGS,
                eventBus::delegate<transitionData>{std::bind(&settingsMenuScreen::onShow, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_GAME,
                eventBus::delegate<transitionData>{std::bind(&settingsMenuScreen::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_ONLINE_GAME,
                eventBus::delegate<transitionData>{std::bind(&settingsMenuScreen::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_GAME_OVER,
                eventBus::delegate<transitionData>{std::bind(&settingsMenuScreen::onHide, this, std::placeholders::_1)}
            },
            {
                NAVIGATION_NETWORK_CONNECT,
                eventBus::delegate<transitionData>{std::bind(&settingsMenuScreen::onHide, this, std::placeholders::_1)}
            },
        };

        void setupPlayerButton();

    public:
        settingsMenuScreen(std::shared_ptr<renderer::renderer> rdr, std::shared_ptr<eventBus::eventBus> events)
            : IScreen(rdr, events)
        {
            for (std::pair<const int, eventBus::delegate<transitionData>> transitionMap : transitionsMap)
            {
                size_t id = events->subscribe<transitionData>(transitionMap.first, transitionMap.second.action);
                transitionsMap[transitionMap.first].uid = id;
            }

            players.resize(2);
            std::stringstream ss;
            for (int i = 0, n = players.size(); i < n; ++i)
            {
                ss << "Player 0" << (i + 1);
                players[i] = ss.str();
                ss.str("");
            }
            playersButtons.resize(12);
        }

        ~settingsMenuScreen() override
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
        void selectPlayerButton(int index) const;
        void deselectPlayerButton(int index) const;
        template <typename T>
        bool editBoxSetup(std::string title, T& data, std::string& newValue);
        void editBoxTearDown(const std::function<void()>& callback);
        void openStringEditBox(std::string title, std::string& data, const std::function<void()>& callback);
        void openSizeTEditBox(std::string title, size_t& data, const std::function<void()>& callback);

        std::vector<std::string>& getPlayers()
        {
            return players;
        }

        std::string& getConfigFilePath()
        {
            return configFilePath;
        }

        int getHandCount() const
        {
            return handCount;
        }

        size_t getSeed() const
        {
            return seed;
        }

    private:
        void updateStartingCardsNumber(int index);
        void updatePlayersCount(int index, int size);
        void enterPlayerEditMode();
        void exitPlayerEditMode();
        template <typename T>
        void configureButton(
            int index,
            int buttonWidth,
            int buttonHeight,
            int positionX,
            int positionY,
            T value,
            std::string prefix,
            std::string suffix,
            std::stringstream& ss,
            std::function<void()> action,
            std::function<void()> actionLeft,
            std::function<void()> actionRight
        );
        template <class T>
        void configurePlayerButton(int index, int buttonWidth, int buttonHeight, int positionX, int positionY, T value,
                                   std::string prefix, std::string suffix, std::stringstream& ss,
                                   std::function<void()> action,
                                   std::function<void()> actionLeft, std::function<void()> actionRight);
    };
}
