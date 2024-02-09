﻿#include "gameScreen.h"

#include "../renderer/elements/card.h"
#include "../renderer/elements/horizontalLayoutGroup.h"
#include "../renderer/elements/text.h"
#include "Cards/ActionTypes/base.h"
#include "Cards/ActionTypes/draw.h"

namespace screens
{
    void gameScreen::show()
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


        popupButton.id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(windowSize.X / 4),
                static_cast<SHORT>(windowSize.Y / 2 - 5)
            },
            COORD{0, 0},
            ' ',
            'r',
            "",
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

        topCardId = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(windowSize.X / 2),
                static_cast<SHORT>(windowSize.Y / 2)
            },
            COORD{
                static_cast<SHORT>(cardSizeX),
                static_cast<SHORT>(cardSizeY)
            },
            ' ',
            'w',
            "top",
            "card"
        );

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
        int baseCardPoolSize = gameManager->getStartHandSize();
        expandCardsPool(baseCardPoolSize);

        if (gameManager->isGameStarted())
        {
            showCurrentPlayerCards();
        }

        rdr->setDirty();
    }

    void gameScreen::hide()
    {
        IScreen::hide();
    }

    void gameScreen::moveUp(input::inputData data)
    {
    }

    void gameScreen::moveDown(input::inputData data)
    {
    }

    void gameScreen::moveLeft(input::inputData data)
    {
    }

    void gameScreen::moveRight(input::inputData data)
    {
    }

    void gameScreen::accept(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (isPopupOpen)
        {
            hidePopup();
            if (popupButton.action != nullptr)
            {
                popupButton.action();
            }
            return;
        }
        if (selectingCards)
        {
            cardListButtons[currentCardButton].action();
            return;
        }
    }

    void gameScreen::cancel(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (isPopupOpen)
        {
            hidePopup();
            if (popupButton.actionLeft != nullptr)
            {
                popupButton.actionLeft();
            }
            return;
        }

        popupButton.action = [this]
        {
            events->fireEvent(NAVIGATION_MAIN_MENU, transitionData());
            hide();
        };
        openWarningPopup(
            "Returning will end the game. Are You Sure?"
        );
    }

    void gameScreen::expandCardsPool(int handSize)
    {
        COORD windowSize = rdr->getWindowSize();
        int border = 5;
        int lastX = border;
        int lastY = windowSize.Y - cardSizeY;
        auto pool = dynamic_cast<elements::horizontalLayoutGroup*>(rdr->getElement(handCardsPoolId));
        int offset = handSize - cardListButtons.size();
        cardListButtons.resize(handSize);
        for (int i = 0; i < offset; ++i)
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
    }

    void gameScreen::hideCard(std::vector<button>::const_reference button)
    {
        auto pool = dynamic_cast<elements::horizontalLayoutGroup*>(rdr->getElement(handCardsPoolId));
        auto cardElement = dynamic_cast<elements::card*>(pool->getElement(button.id));
        cardElement->setSize(COORD{0, 0});
    }

    void gameScreen::showCurrentPlayerCards()
    {
        auto pool = dynamic_cast<elements::horizontalLayoutGroup*>(rdr->getElement(handCardsPoolId));
        std::list<cards::ICard*> hand = gameManager->getCurrentPlayer()->getHand();
        int handSize = hand.size();
        if (handSize > cardListButtons.size())
        {
            expandCardsPool(handSize);
        }
        int i = 0;
        for (auto card : hand)
        {
            auto cardElement = dynamic_cast<elements::card*>(pool->getElement(cardListButtons[i].id));
            setCardData(cardElement, card);
            i++;
        }
        for (int n = cardListButtons.size(); i < n; ++i)
        {
            hideCard(cardListButtons[i]);
        }
        updateTopCard();
        updateCurrentPlayerName();
    }

    void gameScreen::setCardData(elements::card* cardElement, cards::ICard* card)
    {
        cardElement->setTitleText(card->typeName());

        if (card->actionType()->isEqual(typeid(cards::actions::base)) ||
            card->actionType()->isEqual(typeid(cards::actions::draw)))
        {
            cardElement->setCenterText(std::to_string(card->Number()));
        }
        else
        {
            cardElement->setCenterText("");
        }

        cardElement->setColor(card->Color());
        cardElement->setSize(
            COORD{
                static_cast<SHORT>(cardSizeX),
                static_cast<SHORT>(cardSizeY)
            }
        );
    }

    void gameScreen::updateTopCard()
    {
        elements::card* topCardElement = dynamic_cast<elements::card*>(rdr->getElement(topCardId));
        cards::ICard* topCard = gameManager->getTopCard();
        setCardData(topCardElement, topCard);

        rdr->isDirty();
    }

    void gameScreen::updateCurrentPlayerName() const
    {
        auto playerName = dynamic_cast<elements::card*>(rdr->getElement(currentPlayerInfoId));
        playerName->setCenterText(gameManager->getCurrentPlayer()->getName());
        rdr->setDirty();
    }

    void gameScreen::hidePopup()
    {
        isPopupOpen = false;
        auto popup = dynamic_cast<elements::card*>(rdr->getElement(popupButton.id));
        popup->setSize(COORD{0, 0});
        popup->setTitleText("");
        popup->setCenterText("");
        rdr->setDirty();
    }

    void gameScreen::openWarningPopup(std::string bodyText)
    {
        isPopupOpen = true;
        COORD windowSize = rdr->getWindowSize();
        auto popup = dynamic_cast<elements::card*>(rdr->getElement(popupButton.id));
        popup->setSize(
            COORD{
                static_cast<SHORT>(windowSize.X / 2),
                static_cast<SHORT>(5)
            }
        );
        popup->setTitleText("Warning");
        popup->setCenterText(bodyText);
        rdr->setDirty();
    }
}
