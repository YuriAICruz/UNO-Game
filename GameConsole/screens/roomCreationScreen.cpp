#include "roomCreationScreen.h"
#include "../renderer/elements/text.h"
#include "../renderer/elements/card.h"

namespace screens
{
    void roomCreationScreen::show()
    {
        IScreen::show();
        rdr->clear();

        COORD winSize = rdr->getWindowSize();
        std::string titleText = "Select Or Create a Room";

        int lastY = 2;
        titleId = rdr->addElement<elements::text>(
            COORD{
                static_cast<SHORT>(winSize.X / 2 - titleText.size() / 2),
                static_cast<SHORT>(lastY),
            },
            ' ',
            'b',
            titleText
        );

        int buttonWidth = 30;
        int buttonHeight = 3;
        int offset = 1;

        int lastX = winSize.X / 2 - buttonWidth - offset;
        lastY += buttonHeight + offset;
        buttons[0].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Create");

        lastX = winSize.X / 2 + offset;
        buttons[1].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Select");

        popup.show();

        selectButton(currentButton);

        rdr->setDirty();
    }

    void roomCreationScreen::hide()
    {
        IScreen::hide();
        popup.hide();
    }

    void roomCreationScreen::moveUp(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.executeActionCancel();
            return;
        }
    }

    void roomCreationScreen::moveDown(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.executeActionCancel();
            return;
        }
    }

    void roomCreationScreen::moveLeft(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.executeActionCancel();
            return;
        }

        deselectButton(currentButton);
        currentButton--;
        if (currentButton < 0)
        {
            currentButton = std::size(buttons) + currentButton;
        }
        selectButton(currentButton);
    }

    void roomCreationScreen::moveRight(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.executeActionCancel();
            return;
        }

        deselectButton(currentButton);
        currentButton = (1 + currentButton) % std::size(buttons);
        selectButton(currentButton);
    }

    void roomCreationScreen::accept(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.executeActionAccept();
            return;
        }
    }

    void roomCreationScreen::cancel(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.executeActionCancel();
            return;
        }
        popup.assignActionAccept([this]
        {
            returnToMainScreen();
        });
        popup.openWarningPopup("Are you sure you want to return to the main menu?");
    }

    void roomCreationScreen::returnToMainScreen()
    {
        netClient->close();
        events->fireEvent(NAVIGATION_MAIN_MENU, transitionData());
        hide();
    }

    void roomCreationScreen::selectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->select();
        rdr->setDirty();
    }

    void roomCreationScreen::deselectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->deselect();
        rdr->setDirty();
    }
}
