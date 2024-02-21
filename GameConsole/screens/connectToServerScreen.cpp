#include <sstream>

#include "connectToServerScreen.h"
#include "../renderer/elements/text.h"
#include "../renderer/elements/card.h"
#include "../renderer/elements/element.h"

namespace screens
{
    void connectToServerScreen::show()
    {
        IScreen::show();
        rdr->clear();

        COORD winSize = rdr->getWindowSize();
        std::string titleText = "Connect to server";

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

        int buttonWidth = 60;
        int buttonHeight = 3;
        int offset = 1;

        int lastX = winSize.X / 2 - buttonWidth / 2;
        lastY += buttonHeight + offset;
        buttons[0].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Server Address");
        buttons[0].action = [this]()
        {
            box.openStringEditBox("Server Address [use tcp://]", serverAddr, [this](std::string)
            {
                updateServerAddress();
            });
        };
        updateServerAddress();

        lastY += buttonHeight + offset;
        buttons[1].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Connect");
        buttons[1].action = [this]
        {
            tryConnectClient();
        };

        lastY += buttonHeight + offset;
        buttons[2].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Return");
        buttons[2].action = [this]
        {
            returnToPreviousScreen();
        };

        popup.show();

        selectButton(currentButton);
        rdr->setDirty();
    }

    void connectToServerScreen::hide()
    {
        IScreen::hide();
        popup.hide();
    }

    void connectToServerScreen::moveUp(input::inputData data)
    {
        if (blockInputs || box.isBlocking())
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.hidePopup();
            popup.executeActionCancel();
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

    void connectToServerScreen::moveDown(input::inputData data)
    {
        if (blockInputs || box.isBlocking())
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.hidePopup();
            popup.executeActionCancel();
            return;
        }

        deselectButton(currentButton);
        currentButton = (currentButton + 1) % std::size(buttons);
        selectButton(currentButton);
        rdr->setDirty();
    }

    void connectToServerScreen::moveLeft(input::inputData data)
    {
        if (blockInputs || box.isBlocking())
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.hidePopup();
            popup.executeActionCancel();
            return;
        }
    }

    void connectToServerScreen::moveRight(input::inputData data)
    {
        if (blockInputs || box.isBlocking())
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.hidePopup();
            popup.executeActionCancel();
            return;
        }
    }

    void connectToServerScreen::accept(input::inputData data)
    {
        if (blockInputs || box.isBlocking())
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.hidePopup();
            popup.executeActionAccept();
            return;
        }

        buttons[currentButton].action();
    }

    void connectToServerScreen::cancel(input::inputData data)
    {
        if (blockInputs || box.isBlocking())
        {
            return;
        }

        returnToPreviousScreen();
    }

    void connectToServerScreen::returnToPreviousScreen()
    {
        events->fireEvent(NAVIGATION_MAIN_MENU, transitionData());
        hide();
    }

    void connectToServerScreen::updateServerAddress()
    {
        auto button = dynamic_cast<elements::card*>(rdr->getElement(buttons[0].id));
        std::stringstream ss;
        ss << "Server Address [" << serverAddr << "]";
        button->setCenterText(ss.str());
        rdr->setDirty();
    }

    void connectToServerScreen::tryConnectClient()
    {
        if(netClient->isRunning())
        {
            netClient->close();
        }
        int result = netClient->start(serverAddr);
        if (result != 0)
        {
            popup.openWarningPopup("Connection Failed.");
            return;
        }
        result = netClient->connectToServer();
        if (result != 0)
        {
            popup.openWarningPopup("Connection Failed, could not access server");
            netClient->close();
            return;
        }
        
        events->fireEvent(NAVIGATION_NETWORK_ROOMS, transitionData());
        hide();
    }

    void connectToServerScreen::selectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->select();
    }

    void connectToServerScreen::deselectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->deselect();
    }
}
