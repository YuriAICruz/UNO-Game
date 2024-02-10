#include "gameOverScreen.h"

#include <sstream>

#include "../renderer/elements/fileRead.h"
#include "../renderer/elements/text.h"

namespace screens
{
    void gameOverScreen::show()
    {
        returnCount = 0;

        IScreen::show();

        rdr->clear();

        COORD windowSize = rdr->getWindowSize();
        rdr->addElement<elements::fileRead>(
            COORD{
                static_cast<SHORT>(windowSize.X / 2 - 55),
                static_cast<SHORT>(windowSize.Y / 2 - 10)
            },
            'g',
            "Data\\Game_Over.txt"
        );

        winnerPlayerNameId = rdr->addElement<elements::text>(
            COORD{
                static_cast<SHORT>(windowSize.X / 2),
                static_cast<SHORT>(windowSize.Y - 10)
            },
            ' ',
            'g',
            ""
        );

        rdr->setDirty();
    }

    void gameOverScreen::moveUp(input::inputData data)
    {
    }

    void gameOverScreen::moveDown(input::inputData data)
    {
    }

    void gameOverScreen::moveLeft(input::inputData data)
    {
    }

    void gameOverScreen::moveRight(input::inputData data)
    {
    }

    void gameOverScreen::accept(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        returnCount++;
        if (returnCount > 1)
        {
            events->fireEvent(NAVIGATION_MAIN_MENU, transitionData());
            hide();
        }
    }

    void gameOverScreen::cancel(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        returnCount++;
        if (returnCount > 1)
        {
            events->fireEvent(NAVIGATION_MAIN_MENU, transitionData());
            hide();
        }
    }

    void gameOverScreen::updateWinningPlayerName()
    {
        auto text = dynamic_cast<elements::text*>(rdr->getElement(winnerPlayerNameId));

        std::stringstream ss;
        ss << "The Player \"" << winnerPlayer->getName() << "\" Won!";
        std::string winText = ss.str();

        COORD windowSize = rdr->getWindowSize();
        text->setPosition(
            COORD{
                static_cast<SHORT>(windowSize.X / 2 - winText.length() / 2),
                static_cast<SHORT>(windowSize.Y - 10)
            }
        );
        text->setText(winText);

        rdr->setDirty();
    }
}
