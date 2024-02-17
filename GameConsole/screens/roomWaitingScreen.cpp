#include "roomWaitingScreen.h"

#include <sstream>

#include "../renderer/elements/card.h"
#include "../renderer/elements/text.h"

namespace screens
{
    void roomWaitingScreen::show()
    {
        IScreen::show();
        rdr->clear();

        std::stringstream ss;
        ss << "Room: " << netClient->getRoomName() << ", waiting for players";
        std::string titleText = ss.str();

        COORD windowSize = rdr->getWindowSize();
        int lastX = windowSize.X / 2 - titleText.size() / 2;
        int lastY = 3;
        titleId = rdr->addElement<elements::text>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY)
            },
            ' ',
            'g',
            titleText
        );

        int buttonWidth = 40;
        int buttonHeight = 3;
        int offset = 1;
        lastX = windowSize.X / 2 - buttonWidth / 2;

        lastY += buttonHeight + offset;
        buttons[0].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY)
            },
            COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight)
            },
            ' ',
            'g',
            "",
            "Ready"
        );

        lastY += buttonHeight + offset;
        buttons[1].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY)
            },
            COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight)
            },
            ' ',
            'g',
            "",
            "Return"
        );

        lastY += (buttonHeight + offset) * 2;
        buttons[2].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY)
            },
            COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight)
            },
            ' ',
            'g',
            "",
            "Start"
        );

        updateStartButton(netClient->getRoom());
        netClient->onRoomUpdate = [this](netcode::room* r)
        {
            updateStartButton(r);
        };
        netGameManager->onRoomGameStarted = [this]()
        {
            goToGameScreen();
        };

        selectButton(currentButton);

        rdr->setDirty();
    }

    void roomWaitingScreen::hide()
    {
        IScreen::hide();
    }

    void roomWaitingScreen::moveUp(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }

        deselectButton(currentButton);
        currentButton = (1 + currentButton) % std::size(buttons);
        selectButton(currentButton);
    }

    void roomWaitingScreen::moveDown(input::inputData data)
    {
        if (blockInputs)
        {
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

    void roomWaitingScreen::moveLeft(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
    }

    void roomWaitingScreen::moveRight(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
    }

    void roomWaitingScreen::accept(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
    }

    void roomWaitingScreen::cancel(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
    }

    void roomWaitingScreen::setGameManager(netGameStateManager* gameManager)
    {
        netGameManager = gameManager;
    }

    void roomWaitingScreen::updateStartButton(netcode::room* room)
    {
        if (blockInputs)
        {
            return;
        }

        auto button = dynamic_cast<elements::card*>(rdr->getElement(buttons[2].id));

        std::stringstream ss;
        ss << "[" << room->count() << "] players in room, Start?";
        button->setCenterText(ss.str());
        rdr->setDirty();
    }

    void roomWaitingScreen::goToGameScreen()
    {
        if (blockInputs)
        {
            return;
        }

        events->fireEvent(NAVIGATION_ONLINE_GAME, transitionData());
        hide();
    }

    void roomWaitingScreen::selectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->select();
        rdr->setDirty();
    }

    void roomWaitingScreen::deselectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->deselect();
        rdr->setDirty();
    }
}
