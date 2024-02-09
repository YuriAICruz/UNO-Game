#include "settingsMenu.h"

#include <sstream>

#include "../renderer/elements/card.h"

namespace screens
{
    void settingsMenu::show()
    {
        IScreen::show();

        rdr->clear();

        setupPlayerButton();

        COORD winSize = rdr->getWindowSize();

        std::string title = "Settings";
        int border = 20;
        int buttonWidth = winSize.X - border * 2;
        int buttonHeight = 3;
        int offset = 1;

        int lastX = winSize.X / 2 - title.length() / 2;
        int lastY = 1;

        size_t titleId = rdr->addElement<elements::text>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            },
            '+',
            'w',
            title
        );

        lastX = border;
        lastY = offset*2;

        std::stringstream ss;
        configureButton(
            0,
            buttonWidth, buttonHeight,
            lastX, lastY,
            handCount,
            "Number of starting cards [", "]",
            ss,
            nullptr,
            [this]
            {
                handCount = max(2, handCount-1);
                updateStartingCardsNumber(0);
            },
            [this]
            {
                handCount = min(12, handCount+1);
                updateStartingCardsNumber(0);
            });

        lastY += buttonHeight + offset;
        ss.str("");
        configureButton(
            1,
            buttonWidth, buttonHeight,
            lastX, lastY,
            configFilePath,
            "Deck configuration json path: \"", "\"",
            ss,
            [this]
            {
                openStringEditBox("Deck configuration json path", configFilePath, [this]
                {
                    std::stringstream ss;
                    ss << "Deck configuration json path: \"" << configFilePath << "\"";
                    auto filePathButton = static_cast<elements::card*>(rdr->getElement(buttons[1].id));
                    filePathButton->setCenterText(ss.str());
                    rdr->setDirty();
                });
            },
            nullptr,
            nullptr
        );

        lastY += buttonHeight + offset;
        configureButton(
            2,
            buttonWidth, buttonHeight,
            lastX, lastY,
            seed,
            "Random Seed: [", "]",
            ss,
            [this]
            {
                openSizeTEditBox("Random Seed", seed, [this]
                {
                    std::stringstream ss;
                    ss << "Random Seed: [" << seed << "]";
                    auto seedButton = static_cast<elements::card*>(rdr->getElement(buttons[2].id));
                    seedButton->setCenterText(ss.str());
                    rdr->setDirty();
                });
            },
            nullptr,
            nullptr
        );

        lastY += buttonHeight + offset;
        configureButton(
            3,
            buttonWidth, buttonHeight,
            lastX, lastY,
            players.size(),
            "Number of Players: [", "]",
            ss,
            [this]
            {
                enterPlayerEditMode();
            },
            [this]
            {
                int lastSize = players.size();
                int size = min(playersButtons.size(), max(2, lastSize - 1));
                if (lastSize == size)
                {
                    return;
                }
                updatePlayersCount(3, size);
            },
            [this]
            {
                int lastSize = players.size();
                int size = min(playersButtons.size(), max(2, lastSize + 1));
                if (lastSize == size)
                {
                    return;
                }
                updatePlayersCount(3, size);
            }
        );
        updatePlayersCount(3, players.size());

        selectButton(currentButton);
        exitPlayerEditMode();
        rdr->setDirty();
    }

    void settingsMenu::moveUp(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }

        if (editingPlayers)
        {
            deselectPlayerButton(currentPlayerButton);
            currentPlayerButton -= 1;
            if (currentPlayerButton < 0)
            {
                currentPlayerButton = std::size(players) + currentPlayerButton;
            }
            selectPlayerButton(currentPlayerButton);
        }
        else
        {
            deselectButton(currentButton);
            currentButton -= 1;
            if (currentButton < 0)
            {
                currentButton = std::size(buttons) + currentButton;
            }
            selectButton(currentButton);
        }
        rdr->setDirty();
    }

    void settingsMenu::moveDown(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }

        if (editingPlayers)
        {
            deselectPlayerButton(currentPlayerButton);
            currentPlayerButton = (currentPlayerButton + 1) % std::size(players);
            selectPlayerButton(currentPlayerButton);
        }
        else
        {
            deselectButton(currentButton);
            currentButton = (currentButton + 1) % std::size(buttons);
            selectButton(currentButton);
        }
        rdr->setDirty();
    }

    void settingsMenu::moveLeft(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (editingPlayers)
        {
            return;
        }
        if (buttons[currentButton].actionLeft != nullptr)
        {
            buttons[currentButton].actionLeft();
        }
    }

    void settingsMenu::moveRight(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (editingPlayers)
        {
            return;
        }
        if (buttons[currentButton].actionRight != nullptr)
        {
            buttons[currentButton].actionRight();
        }
    }

    void settingsMenu::accept(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (editingPlayers)
        {
            if (playersButtons[currentPlayerButton].action != nullptr)
            {
                playersButtons[currentPlayerButton].action();
            }
        }
        else
        {
            if (buttons[currentButton].action != nullptr)
            {
                buttons[currentButton].action();
            }
        }
    }

    void settingsMenu::cancel(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (editingPlayers)
        {
            exitPlayerEditMode();
        }
        else
        {
            events->fireEvent(NAVIGATION_MAIN_MENU, transitionData());
            hide();
        }
    }

    void settingsMenu::selectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->select();
    }

    void settingsMenu::deselectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->deselect();
    }

    void settingsMenu::selectPlayerButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(playersButtons[index].id));
        button->select();
    }

    void settingsMenu::deselectPlayerButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(playersButtons[index].id));
        button->deselect();
    }

    template <typename T>
    bool settingsMenu::editBoxSetup(std::string title, T& data, std::string& newValue)
    {
        blockInputs = true;
        rdr->blank();
        std::cout << "Enter a new value for " << title << ", you can cancel by typing 'quit'" << "\n";
        std::cout << "Current value is: " << data << "\n";
        std::getline(std::cin, newValue);
        if (newValue != "quit")
        {
            return true;
        }
        return false;
    }

    void settingsMenu::editBoxTearDown(const std::function<void()>& callback)
    {
        callback();
        blockInputs = false;
    }

    void settingsMenu::openStringEditBox(std::string title, std::string& data, const std::function<void()>& callback)
    {
        std::string newValue;
        if (editBoxSetup(title, data, newValue))
        {
            data = newValue;
        }
        editBoxTearDown(callback);
    }

    void settingsMenu::openSizeTEditBox(std::string title, size_t& data, const std::function<void()>& callback)
    {
        std::string newValue;
        if (editBoxSetup(title, data, newValue))
        {
            try
            {
                data = std::stoul(newValue);
            }
            catch (const std::invalid_argument& e)
            {
                std::cerr << "ERROR: Invalid argument: " << e.what() << std::endl;
            } catch (const std::out_of_range& e)
            {
                std::cerr << "ERROR: Out of range: " << e.what() << std::endl;
            }
        }
        editBoxTearDown(callback);
    }

    void settingsMenu::updateStartingCardsNumber(int index)
    {
        std::stringstream ss;
        ss << "Number of starting cards [" << handCount << "]";
        auto cardsNumbersButton = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        cardsNumbersButton->setCenterText(ss.str());
        rdr->setDirty();
    }

    void settingsMenu::setupPlayerButton()
    {
        int size = 12;
        playersButtons.resize(size);

        std::stringstream ss;
        int playerButtonWidth = 1;
        int playerButtonHeight = 1;
        int lastX = 0;
        int lastY = 0;

        for (int i = 0; i < size; ++i)
        {
            configurePlayerButton(
                i,
                playerButtonWidth, playerButtonHeight,
                lastX, lastY,
                "Not Setup",
                "Player: ", "",
                ss,
                [this, i]
                {
                    openStringEditBox("Enter Player Name:", players[i], [this, i]
                    {
                        auto playerButton = static_cast<elements::card*>(rdr->getElement(playersButtons[i].id));
                        playerButton->setCenterText(players[i]);
                        rdr->setDirty();
                    });
                },
                nullptr,
                nullptr
            );
        }
    }

    void settingsMenu::updatePlayersCount(int index, int size)
    {
        players.resize(size);
        std::stringstream ss;
        ss << "Number of Players: [" << size << "]";
        auto playersButton = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        playersButton->setCenterText(ss.str());

        auto windowSize = rdr->getWindowSize();
        int offset = 0;
        int border = 1;
        int upperSide = 16;
        int columnCount = 4;
        int lineCount = 3;
        int playerButtonWidth = windowSize.X / columnCount - border * 2 - offset;
        int playerButtonHeight = max(3, (windowSize.Y - border * 2 - upperSide) / (size) - offset);
        int column = 0;
        int line = 0;
        int lastX = border;
        int lastY = upperSide + border;

        int i = 0;
        for (; i < size; ++i)
        {
            elements::card* playerButton = static_cast<elements::card*>(rdr->getElement(playersButtons[i].id));
            playerButton->setPosition(COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY)
            });
            playerButton->setSize(COORD{
                static_cast<SHORT>(playerButtonWidth),
                static_cast<SHORT>(playerButtonHeight)
            });
            playerButton->setCenterText(players[i]);
            line++;
            if (line >= lineCount)
            {
                line = 0;
                column++;
            }
            lastX = border + column * playerButtonWidth + offset;
            lastY = upperSide + border + line * playerButtonHeight + offset;
        }
        for (int n = playersButtons.size(); i < n; ++i)
        {
            elements::card* playerButton = static_cast<elements::card*>(rdr->getElement(playersButtons[i].id));
            playerButton->setPosition(COORD{0, 0});
            playerButton->setSize(COORD{0, 0});
            playerButton->setCenterText("");
        }

        rdr->setDirty();
    }

    void settingsMenu::enterPlayerEditMode()
    {
        editingPlayers = true;
        deselectButton(currentButton);
        selectPlayerButton(currentPlayerButton);
        rdr->setDirty();
    }

    void settingsMenu::exitPlayerEditMode()
    {
        editingPlayers = false;
        selectButton(currentButton);
        deselectPlayerButton(currentPlayerButton);
        rdr->setDirty();
    }

    template <typename T>
    void settingsMenu::configureButton(
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
    )
    {
        ss.str("");
        ss << prefix << value << suffix;

        buttons[index].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(positionX),
                static_cast<SHORT>(positionY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            },
            '+',
            'g',
            "",
            ss.str()
        );
        buttons[index].action = action;
        buttons[index].actionLeft = actionLeft;
        buttons[index].actionRight = actionRight;
    }

    template <typename T>
    void settingsMenu::configurePlayerButton(
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
    )
    {
        ss.str("");
        ss << prefix << value << suffix;

        playersButtons[index].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(positionX),
                static_cast<SHORT>(positionY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            },
            '+',
            'g',
            "",
            ss.str()
        );
        playersButtons[index].action = action;
        playersButtons[index].actionLeft = actionLeft;
        playersButtons[index].actionRight = actionRight;
    }
}
