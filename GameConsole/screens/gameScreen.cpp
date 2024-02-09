#include "gameScreen.h"

#include "../renderer/elements/card.h"
#include "../renderer/elements/horizontalLayoutGroup.h"
#include "../renderer/elements/text.h"

void screens::gameScreen::show()
{
    IScreen::show();

    rdr->clear();

    std::string title = "Game Running";
    COORD windowSize = rdr->getWindowSize();
    int lastX = windowSize.X / 2 - title.length() / 2;
    int lastY = 2;
    int offset = 2;

    rdr->addElement<elements::text>(
        COORD{
            static_cast<SHORT>(lastX),
            static_cast<SHORT>(lastY)
        },
        '+',
        'b',
        title);

    lastY += offset;
    int border = 5;
    int sideMenuWidth = 15;

    lastX = border;

    currentPlayerInfoId = rdr->addElement<elements::card>(
        COORD{
            static_cast<SHORT>(lastX),
            static_cast<SHORT>(lastY)
        },
        COORD{
            static_cast<SHORT>(windowSize.X - border * 2 - sideMenuWidth),
            static_cast<SHORT>(5)
        },
        ' ',
        'g',
        "Current Player",
        ""
    );

    int buttonWidth = sideMenuWidth;
    int buttonHeight = 3;
    lastX = windowSize.X - buttonWidth - border / 2;
    optionButtons[0].id = rdr->addElement<elements::card>(
        COORD{
            static_cast<SHORT>(lastX),
            static_cast<SHORT>(lastY)
        },
        COORD{
            static_cast<SHORT>(buttonWidth),
            static_cast<SHORT>(buttonHeight)
        },
        ' ',
        'y',
        "Pass",
        ""
    );
    lastY += buttonHeight;
    optionButtons[1].id = rdr->addElement<elements::card>(
        COORD{
            static_cast<SHORT>(lastX),
            static_cast<SHORT>(lastY)
        },
        COORD{
            static_cast<SHORT>(buttonWidth),
            static_cast<SHORT>(buttonHeight)
        },
        ' ',
        'y',
        "Play Card",
        ""
    );
    lastY += buttonHeight;
    optionButtons[2].id = rdr->addElement<elements::card>(
        COORD{
            static_cast<SHORT>(lastX),
            static_cast<SHORT>(lastY)
        },
        COORD{
            static_cast<SHORT>(buttonWidth),
            static_cast<SHORT>(buttonHeight)
        },
        ' ',
        'y',
        "Draw",
        ""
    );

    int cardSizeX = 8;
    int cardSizeY = 6;

    lastX = border;
    lastY = windowSize.Y - cardSizeY;
    handCardsPoolId = rdr->addElement<elements::horizontalLayoutGroup>(
        COORD{
            static_cast<SHORT>(lastX),
            static_cast<SHORT>(lastY)
        },
        ' ',
        'w',
        1
    );
    auto pool = dynamic_cast<elements::horizontalLayoutGroup*>(rdr->getElement(handCardsPoolId));
    int baseCardPoolSize = gameManager->getStartHandSize();
    cardListButtons.resize(baseCardPoolSize);
    for (int i = 0; i < baseCardPoolSize; ++i)
    {
        cardListButtons[i].id = pool->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY)
            },
            COORD{
                static_cast<SHORT>(cardSizeX),
                static_cast<SHORT>(cardSizeY)
            },
            ' ',
            'w',
            "type",
            "num");
    }
    pool->resize();

    ShowCurrentPlayerCards();

    rdr->setDirty();
}

void screens::gameScreen::hide()
{
    IScreen::hide();
}

void screens::gameScreen::moveUp(input::inputData data)
{
}

void screens::gameScreen::moveDown(input::inputData data)
{
}

void screens::gameScreen::moveLeft(input::inputData data)
{
}

void screens::gameScreen::moveRight(input::inputData data)
{
}

void screens::gameScreen::accept(input::inputData data)
{
}

void screens::gameScreen::cancel(input::inputData data)
{
}

void screens::gameScreen::ShowCurrentPlayerCards()
{
}

void screens::gameScreen::UpdateCurrentPlayerName()
{
    auto playerName = dynamic_cast<elements::card*>(rdr->getElement(currentPlayerInfoId));
    playerName->setCenterText(gameManager->getCurrentPlayer()->getName());
    rdr->setDirty();
}
