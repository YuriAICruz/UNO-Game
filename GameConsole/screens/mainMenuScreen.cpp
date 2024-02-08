#include "mainMenuScreen.h"
#include "../renderer/elements/card.h"

namespace screens
{
    void mainMenuScreen::show()
    {
        rdr->clear();
        COORD winSize = rdr->getWindowSize();

        std::string title = "UNO - A Card Gaming experience";
        int buttonWidth = title.length() - 5;
        int buttonHeight = 3;
        int offset = 1;

        int lastX = winSize.X / 2 - buttonWidth / 2;
        int lastY = winSize.Y / 2 + buttonHeight / 2;

        titleId = rdr->addElement<elements::text>(
            COORD{
                static_cast<SHORT>(winSize.X / 2 - title.length() / 2),
                static_cast<SHORT>(lastY - 5),
            },
            '+',
            'w',
            title
        );
        buttons[0] = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Start Game");

        lastY = lastY + buttonHeight + offset;
        buttons[1] = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Settings");

        lastY = lastY + buttonHeight + offset;
        buttons[2] = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Exit");

        selectButton(currentButton);
    }

    void mainMenuScreen::moveUp(input::inputData data)
    {
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

    void mainMenuScreen::deselectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index]));
        button->deselect();
    }

    void mainMenuScreen::selectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index]));
        button->select();
    }
}
