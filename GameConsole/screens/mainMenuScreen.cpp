#include "mainMenuScreen.h"

#include "transitionData.h"
#include "../renderer/elements/card.h"
#include "../renderer/elements/fileRead.h"

namespace screens
{
    void mainMenuScreen::show()
    {
        IScreen::show();

        rdr->clear();
        COORD winSize = rdr->getWindowSize();

        int buttonWidth = 40;
        int buttonHeight = 3;
        int offset = 1;

        int lastX = winSize.X / 2 - buttonWidth / 2;
        int lastY = winSize.Y / 2 - buttonHeight / 2;

        titleId = rdr->addElement<elements::fileRead>(
            COORD{
                static_cast<SHORT>(winSize.X / 2 - 16),
                static_cast<SHORT>(4),
            },
            'b',
            "Data\\Title.txt"
        );

        buttons[0].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Start Offline Game");
        buttons[0].action = [this]
        {
            events->fireEvent(NAVIGATION_GAME, transitionData());
        };

        lastY = lastY + buttonHeight + offset;
        buttons[1].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Start Multiplayer Game");
        buttons[1].action = [this]
        {
            events->fireEvent(NAVIGATION_NETWORK_CONNECT, transitionData());
        };

        lastY = lastY + buttonHeight + offset;
        buttons[2].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Settings");
        buttons[2].action = [this]
        {
            events->fireEvent(NAVIGATION_SETTINGS, transitionData());
        };

        lastY = lastY + buttonHeight + offset;
        buttons[3].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Exit");
        buttons[3].action = [this]
        {
            hide();
            rdr->exit();
        };

        setButtonsSelectionColor();
        selectButton(currentButton);
        rdr->setDirty();
    }

    void mainMenuScreen::moveUp(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }

        deselectButton(currentButton);
        currentButton -= 1;
        if (currentButton < 0)
        {
            currentButton = std::size(buttons) + currentButton;
        }
        selectButton(currentButton);
        rdr->setDirty();
    }

    void mainMenuScreen::moveDown(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }

        deselectButton(currentButton);
        currentButton = (currentButton + 1) % std::size(buttons);
        selectButton(currentButton);
        rdr->setDirty();
    }

    void mainMenuScreen::moveLeft(input::inputData data)
    {
    }

    void mainMenuScreen::moveRight(input::inputData data)
    {
    }

    void mainMenuScreen::accept(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        buttons[currentButton].action();
    }

    void mainMenuScreen::cancel(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        rdr->exit();
    }

    void mainMenuScreen::setButtonsSelectionColor()
    {
        char color = 'b';
        for (auto button : buttons)
        {
            (dynamic_cast<elements::card*>(rdr->getElement(button.id)))->setSelectionColor(color);
        }
    }

    void mainMenuScreen::selectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->select();
    }

    void mainMenuScreen::deselectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->deselect();
    }
}
