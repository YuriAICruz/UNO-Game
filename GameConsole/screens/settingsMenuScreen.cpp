#include "settingsMenuScreen.h"

#include <sstream>

#include "../renderer/elements/card.h"

namespace screens
{
    void settingsMenuScreen::show()
    {
        IScreen::show();

        rdr->clear();

        setupPlayerButton();

        COORD winSize = rdr->getWindowSize();

        std::string title = "Settings";
        int border = 20;
        int buttonWidth = winSize.X - border * 2;
        int buttonHeight = 3;
        int offset = 0;

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
        lastY = offset * 2;

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
        configureButton(
            1,
            buttonWidth, buttonHeight,
            lastX, lastY,
            showWarningEachTurn,
            "Show warning each turn [", "]",
            ss,
            [this]
            {
                showWarningEachTurn = !showWarningEachTurn;
                std::stringstream ss;
                ss << "Show warning each turn [" << showWarningEachTurn << "]";
                auto button = dynamic_cast<elements::card*>(rdr->getElement(buttons[1].id));
                button->setCenterText(ss.str());
                rdr->setDirty();
            },
            nullptr,
            nullptr
        );

        lastY += buttonHeight + offset;
        ss.str("");
        configureButton(
            2,
            buttonWidth, buttonHeight,
            lastX, lastY,
            configFilePath,
            "Deck configuration json path: \"", "\"",
            ss,
            [this]
            {
                box.openStringEditBox("Deck configuration json path", configFilePath, [this](std::string)
                {
                    std::stringstream ss;
                    ss << "Deck configuration json path: \"" << configFilePath << "\"";
                    auto filePathButton = static_cast<elements::card*>(rdr->getElement(buttons[2].id));
                    filePathButton->setCenterText(ss.str());
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
            seed,
            "Random Seed: [", "]",
            ss,
            [this]
            {
                box.openSizeTEditBox("Random Seed", seed, [this](std::string)
                {
                    std::stringstream ss;
                    ss << "Random Seed: [" << seed << "]";
                    auto seedButton = static_cast<elements::card*>(rdr->getElement(buttons[3].id));
                    seedButton->setCenterText(ss.str());
                    rdr->setDirty();
                });
            },
            nullptr,
            nullptr
        );

        lastY += buttonHeight + offset;
        configureButton(
            4,
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
                updatePlayersCount(4, size);
            },
            [this]
            {
                int lastSize = players.size();
                int size = min(playersButtons.size(), max(2, lastSize + 1));
                if (lastSize == size)
                {
                    return;
                }
                updatePlayersCount(4, size);
            }
        );

        updatePlayersCount(4, players.size());

        setButtonsSelectionColor();
        selectButton(currentButton);
        exitPlayerEditMode();
        rdr->setDirty();
    }

    void settingsMenuScreen::moveUp(input::inputData data)
    {
        if (blockInputs || box.isBlocking())
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

    void settingsMenuScreen::moveDown(input::inputData data)
    {
        if (blockInputs || box.isBlocking())
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

    void settingsMenuScreen::moveLeft(input::inputData data)
    {
        if (blockInputs || box.isBlocking())
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

    void settingsMenuScreen::moveRight(input::inputData data)
    {
        if (blockInputs || box.isBlocking())
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

    void settingsMenuScreen::accept(input::inputData data)
    {
        if (blockInputs || box.isBlocking())
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

    void settingsMenuScreen::cancel(input::inputData data)
    {
        if (blockInputs || box.isBlocking())
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

    void settingsMenuScreen::selectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->select();
    }

    void settingsMenuScreen::deselectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->deselect();
    }

    void settingsMenuScreen::selectPlayerButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(playersButtons[index].id));
        button->select();
    }

    void settingsMenuScreen::deselectPlayerButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(playersButtons[index].id));
        button->deselect();
    }

    void settingsMenuScreen::updateStartingCardsNumber(int index)
    {
        std::stringstream ss;
        ss << "Number of starting cards [" << handCount << "]";
        auto cardsNumbersButton = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        cardsNumbersButton->setCenterText(ss.str());
        rdr->setDirty();
    }

    void settingsMenuScreen::setupPlayerButton()
    {
        int size = maxPlayers;
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
                    box.openStringEditBox("Enter Player Name:", players[i], [this, i](std::string)
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

    void settingsMenuScreen::updatePlayersCount(int index, int size)
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

        setButtonsSelectionColor();
        rdr->setDirty();
    }

    void settingsMenuScreen::enterPlayerEditMode()
    {
        editingPlayers = true;
        deselectButton(currentButton);
        selectPlayerButton(currentPlayerButton);
        rdr->setDirty();
    }

    void settingsMenuScreen::exitPlayerEditMode()
    {
        editingPlayers = false;
        selectButton(currentButton);
        deselectPlayerButton(currentPlayerButton);
        rdr->setDirty();
    }

    template <typename T>
    void settingsMenuScreen::configureButton(
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

    void settingsMenuScreen::setButtonsSelectionColor()
    {
        char color = 'b';
        for (auto button : buttons)
        {
            (dynamic_cast<elements::card*>(rdr->getElement(button.id)))->setSelectionColor(color);
        }
        for (auto playerButton : playersButtons)
        {
            (dynamic_cast<elements::card*>(rdr->getElement(playerButton.id)))->setSelectionColor(color);
        }
    }

    template <typename T>
    void settingsMenuScreen::configurePlayerButton(
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
