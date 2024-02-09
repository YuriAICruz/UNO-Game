#include "settingsMenu.h"
#include "../renderer/elements/card.h"

namespace screens
{
    void settingsMenu::show()
    {
        IScreen::show();

        rdr->clear();
        COORD winSize = rdr->getWindowSize();

        std::string title = "Settings";
        int buttonWidth = 40;
        int buttonHeight = 3;
        int offset = 1;

        int lastX = winSize.X / 2 - buttonWidth / 2;
        int lastY = 5;

        size_t titleId = rdr->addElement<elements::text>(
            COORD{
                static_cast<SHORT>(winSize.X / 2 - title.length() / 2),
                static_cast<SHORT>(lastY),
            },
            '+',
            'w',
            title
        );

        lastX = 10;
        lastY += offset + buttonHeight + offset;
        buttons[0].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            },
            '+',
            'g',
            "",
            "Number of cards {7}"
        );
        buttons[0].action = [this]
        {
            auto cardsNumbersButton = static_cast<elements::card*>(rdr->getElement(buttons[0].id));
            cardsNumbersButton->setCenterText("Number of cards {9} [mod]");
            rdr->setDirty();
        };

        selectButton(currentButton);
        rdr->setDirty();
    }

    void settingsMenu::moveUp(input::inputData data)
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

    void settingsMenu::moveDown(input::inputData data)
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

    void settingsMenu::moveLeft(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        buttons[currentButton].actionLeft();
    }

    void settingsMenu::moveRight(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        buttons[currentButton].actionRight();
    }

    void settingsMenu::accept(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        buttons[currentButton].action();
    }

    void settingsMenu::cancel(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        events->fireEvent(NAVIGATION_MAIN_MENU, transitionData());
        hide();
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
}
