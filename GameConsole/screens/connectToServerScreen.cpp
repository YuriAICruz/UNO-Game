#include "connectToServerScreen.h"

#include <sstream>

#include "../renderer/elements/text.h"
#include "../renderer/elements/card.h"
#include "../renderer/elements/element.h"

namespace elements
{
    class card;
}

namespace screens
{
    void connectToServerScreen::show()
    {
        IScreen::show();
        rdr->clear();

        COORD winSize = rdr->getWindowSize();
        std::string titleText = "Connect to server";

        int lastX = 0;
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

        lastY += buttonHeight + offset;
        buttons[0].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Server Address");
        updateServerAddress();

        rdr->setDirty();
    }

    void connectToServerScreen::moveUp(input::inputData data)
    {
    }

    void connectToServerScreen::moveDown(input::inputData data)
    {
    }

    void connectToServerScreen::moveLeft(input::inputData data)
    {
    }

    void connectToServerScreen::moveRight(input::inputData data)
    {
    }

    void connectToServerScreen::accept(input::inputData data)
    {
    }

    void connectToServerScreen::cancel(input::inputData data)
    {
    }

    void connectToServerScreen::updateServerAddress()
    {
        auto button = dynamic_cast<elements::card*>(rdr->getElement(buttons[0].id));
        std::stringstream ss;
        ss << "Server Address [" << serverAddr << "]";
        button->setCenterText(ss.str());
    }
}
