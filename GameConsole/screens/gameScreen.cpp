#include "gameScreen.h"

#include "../renderer/elements/card.h"
#include "../renderer/elements/horizontalLayoutGroup.h"
#include "../renderer/elements/text.h"
#include "Cards/ActionTypes/base.h"
#include "Cards/ActionTypes/draw.h"
#include "../renderer/elements/fileRead.h"

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
        optionButtons[0].action = [this]
        {
            tryToPass();
        };

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
            "Yell UNO",
            ""
        );
        optionButtons[1].action = [this]
        {
            tryYellUno();
        };

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
        optionButtons[2].action = [this]
        {
            tryDrawMoreCards();
        };

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

        switchToCards();


        unoYell[0].id = rdr->addElement<elements::frame>(
            COORD{
                static_cast<SHORT>(windowSize.X + 100),
                static_cast<SHORT>(windowSize.Y + 100)
            }, COORD{32, 9}, '.', 'p');
        unoYell[1].id = rdr->addElement<elements::fileRead>(
            COORD{
                static_cast<SHORT>(windowSize.X + 100),
                static_cast<SHORT>(windowSize.Y + 100)
            }, 'b', "Data\\UNO.txt");

        rdr->setDirty();
    }

    void gameScreen::hide()
    {
        IScreen::hide();
    }

    void gameScreen::moveUp(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (unoPopup)
        {
            hideUnoPopup();
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
        if (selectingCards)
        {
            switchToOptions();
            return;
        }
        if (selectingOptions)
        {
            deselectOptionButton(currentOptionButton);
            currentOptionButton = max(0, currentOptionButton-1);
            selectOptionButton(currentOptionButton);
            rdr->setDirty();
            return;
        }
    }

    void gameScreen::moveDown(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (unoPopup)
        {
            hideUnoPopup();
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
        if (selectingOptions)
        {
            int size = std::size(optionButtons);
            if (currentOptionButton >= size - 1)
            {
                switchToCards();
                return;
            }
            deselectOptionButton(currentOptionButton);
            currentOptionButton = min(size, currentOptionButton+1);
            selectOptionButton(currentOptionButton);
            rdr->setDirty();
            return;
        }
    }

    void gameScreen::moveLeft(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (unoPopup)
        {
            hideUnoPopup();
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
        if (selectingCards)
        {
            int handSize = gameManager->getCurrentPlayer()->getHand().size();
            deselectCardButton(currentCardButton);
            currentCardButton -= 1;
            if (currentCardButton < 0)
            {
                currentCardButton = handSize + currentCardButton;
            }
            selectCardButton(currentCardButton);
            rdr->setDirty();
            return;
        }
    }

    void gameScreen::moveRight(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (unoPopup)
        {
            hideUnoPopup();
        }
        if (selectingCards)
        {
            int handSize = gameManager->getCurrentPlayer()->getHand().size();
            deselectCardButton(currentCardButton);
            currentCardButton = (currentCardButton + 1) % handSize;
            selectCardButton(currentCardButton);
            rdr->setDirty();
            return;
        }
    }

    void gameScreen::accept(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (unoPopup)
        {
            hideUnoPopup();
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
            selectCard(currentCardButton);
            return;
        }
        if (selectingOptions)
        {
            if (optionButtons[currentOptionButton].action != nullptr)
            {
                optionButtons[currentOptionButton].action();
            }
            return;
        }
    }

    void gameScreen::cancel(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (unoPopup)
        {
            hideUnoPopup();
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

        popupButton.actionLeft = nullptr;
        popupButton.actionRight = nullptr;
        popupButton.action = [this]
        {
            events->fireEvent(NAVIGATION_MAIN_MENU, transitionData());
            hide();
        };
        openWarningPopup(
            "Returning will end the game. Are You Sure?"
        );
    }

    void gameScreen::hideUnoPopup()
    {
        unoPopup = false;
        auto winSize = rdr->getWindowSize();
        COORD outBoundPos = COORD{
            static_cast<SHORT>(winSize.X + 100),
            static_cast<SHORT>(winSize.Y + 100)
        };
        for (auto element : unoYell)
        {
            auto b = rdr->getElement(element.id);
            b->setPosition(outBoundPos);
        }
        rdr->setDirty();
    }

    void gameScreen::showUnoPopup()
    {
        unoPopup = true;
        auto winSize = rdr->getWindowSize();

        auto b = rdr->getElement(unoYell[0].id);
        b->setPosition(COORD{
            static_cast<SHORT>(winSize.X / 2 - 15),
            static_cast<SHORT>(winSize.Y / 2 - 4)
        });
        b = rdr->getElement(unoYell[1].id);
        b->setPosition(COORD{
            static_cast<SHORT>(winSize.X / 2 + 1 - 15),
            static_cast<SHORT>(winSize.Y / 2 + 1 - 4)
        });
        rdr->setDirty();
    }

    void gameScreen::tryYellUno()
    {
        if (gameManager->canYellUno())
        {
            gameManager->yellUno();

            showUnoPopup();
            return;
        }
        popupButton.action = nullptr;
        popupButton.actionLeft = nullptr;
        popupButton.actionRight = nullptr;
        openWarningPopup("You can only yell UNO with two cards");
    }

    void gameScreen::onUnoPenalty(gameEventData data)
    {
        popupButton.action = nullptr;
        popupButton.actionLeft = nullptr;
        popupButton.actionRight = nullptr;
        openWarningPopup("You forgot to yell UNO, received two cards");
    }

    void gameScreen::onGameEnded(gameEventData data)
    {
        events->fireEvent(NAVIGATION_GAME_OVER, transitionData(data.player));
        hide();
    }

    void gameScreen::tryToPass()
    {
        if (gameManager->canSkipTurn())
        {
            gameManager->skipTurn();
            showCurrentPlayerCards();
            return;
        }
        popupButton.action = nullptr;
        popupButton.actionLeft = nullptr;
        popupButton.actionRight = nullptr;
        openWarningPopup("You can only skip if you already draw a card");
    }

    void gameScreen::tryDrawMoreCards()
    {
        if (gameManager->playerHasValidCardOnHand(gameManager->getCurrentPlayer()))
        {
            popupButton.action = [this]
            {
                drawOneCard();
            };
            popupButton.actionLeft = nullptr;
            popupButton.actionRight = nullptr;
            openWarningPopup("You have playable cards, are you sure?");
            return;
        }
        if (gameManager->canDrawCard())
        {
            drawOneCard();
            return;
        }
        popupButton.action = nullptr;
        popupButton.actionLeft = nullptr;
        popupButton.actionRight = nullptr;
        openWarningPopup("You can only draw on card per turn, you can skip now");
    }

    void gameScreen::drawOneCard()
    {
        gameManager->makePlayerDraw(gameManager->getCurrentPlayer(), 1);
        showCurrentPlayerCards();
    }

    void gameScreen::expandCardsPool(int handSize)
    {
        COORD windowSize = rdr->getWindowSize();
        int border = 5;
        int lastX = border;
        int lastY = windowSize.Y - cardSizeY;
        auto pool = dynamic_cast<elements::horizontalLayoutGroup*>(rdr->getElement(handCardsPoolId));
        int currentListSize = cardListButtons.size();
        cardListButtons.resize(handSize);
        for (int i = currentListSize; i < handSize; ++i)
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
        cardElement->setTitleText("");
        cardElement->setCenterText("");
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

        if (currentCardButton >= hand.size())
        {
            deselectCardButton(currentCardButton);

            currentCardButton = 0;

            if (selectingCards)
            {
                selectCardButton(currentCardButton);
            }
        }

        rdr->setDirty();
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

    void gameScreen::selectCard(int index)
    {
        auto player = gameManager->getCurrentPlayer();
        auto card = player->pickCard(index);
        if (gameManager->tryExecutePlayerAction(card))
        {
            showCurrentPlayerCards();
            return;
        }
        player->receiveCard(card);
        showCurrentPlayerCards();
    }

    void gameScreen::switchToCards()
    {
        selectingCards = true;
        selectingOptions = false;
        selectCardButton(currentCardButton);
        deselectOptionButton(currentOptionButton);
        rdr->setDirty();
    }

    void gameScreen::switchToOptions()
    {
        int size = std::size(optionButtons);
        currentOptionButton = size - 1;
        selectingCards = false;
        selectingOptions = true;
        deselectCardButton(currentCardButton);
        selectOptionButton(currentOptionButton);
        rdr->setDirty();
    }

    void gameScreen::selectCardButton(int index) const
    {
        auto pool = dynamic_cast<elements::horizontalLayoutGroup*>(rdr->getElement(handCardsPoolId));
        auto button = static_cast<elements::card*>(pool->getElement(cardListButtons[index].id));
        button->select();
    }

    void gameScreen::deselectCardButton(int index) const
    {
        auto pool = dynamic_cast<elements::horizontalLayoutGroup*>(rdr->getElement(handCardsPoolId));
        auto button = static_cast<elements::card*>(pool->getElement(cardListButtons[index].id));
        button->deselect();
    }

    void gameScreen::selectOptionButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(optionButtons[index].id));
        button->select();
    }

    void gameScreen::deselectOptionButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(optionButtons[index].id));
        button->deselect();
    }
}
