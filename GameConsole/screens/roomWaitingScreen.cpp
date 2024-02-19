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
            "Number of Starting Cards []"
        );
        buttons[0].actionLeft = [this]
        {
            decreaseStartingCards();
        };
        buttons[0].actionRight = [this]
        {
            increaseStartingCards();
        };
        updateStatingCardsCount();

        lastY += (buttonHeight + offset) * 2;
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
            "Start"
        );
        buttons[1].action = [this]
        {
            startRoomGame();
        };

        lastY += buttonHeight + offset;
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
            "Return"
        );
        buttons[2].action = [this]
        {
            tryExitRoom();
        };

        updateStartButton(netClient->getUpdatedRoom());
        netClient->onRoomUpdate = [this](netcode::room* r)
        {
            updateStartButton(r);
        };
        netGameManager->onRoomGameStarted = [this]()
        {
            goToGameScreen();
        };

        popup.show();

        selectButton(currentButton);

        rdr->setDirty();
    }

    void roomWaitingScreen::hide()
    {
        IScreen::hide();
        popup.hide();
    }

    void roomWaitingScreen::moveUp(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.executeActionCancel();
            popup.hidePopup();
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

    void roomWaitingScreen::moveDown(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.executeActionCancel();
            popup.hidePopup();
            return;
        }

        deselectButton(currentButton);
        currentButton = (1 + currentButton) % std::size(buttons);
        selectButton(currentButton);
    }

    void roomWaitingScreen::moveLeft(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.executeActionCancel();
            popup.hidePopup();
            return;
        }

        if (buttons[currentButton].actionLeft != nullptr)
        {
            buttons[currentButton].actionLeft();
        }
    }

    void roomWaitingScreen::moveRight(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.executeActionCancel();
            popup.hidePopup();
            return;
        }

        if (buttons[currentButton].actionRight != nullptr)
        {
            buttons[currentButton].actionRight();
        }
    }

    void roomWaitingScreen::accept(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.executeActionAccept();
            popup.hidePopup();
            return;
        }

        if (buttons[currentButton].action != nullptr)
        {
            buttons[currentButton].action();
        }
    }

    void roomWaitingScreen::cancel(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        if (popup.isOpen())
        {
            popup.executeActionCancel();
            popup.hidePopup();
            return;
        }

        tryExitRoom();
    }

    void roomWaitingScreen::setGameManager(netGameStateManager* gameManager)
    {
        netGameManager = gameManager;
    }

    void roomWaitingScreen::setGameSettings(std::string cardsPath, size_t seed)
    {
        this->cardsPath = cardsPath;
        this->seed = seed;
    }

    void roomWaitingScreen::updateStartButton(netcode::room* room)
    {
        if (blockInputs)
        {
            return;
        }

        auto button = dynamic_cast<elements::card*>(rdr->getElement(buttons[1].id));

        std::stringstream ss;
        ss << "[" << room->count() << "] players in room, Start?";
        button->setCenterText(ss.str());
        rdr->setDirty();
    }

    void roomWaitingScreen::tryExitRoom()
    {
        popup.clearActions();
        popup.assignActionAccept([this]
        {
            exitRoomAndReturnToMainScreen();
        });
        popup.openWarningPopup("Are you sure you want to return to the main menu?");
    }

    void roomWaitingScreen::exitRoomAndReturnToMainScreen()
    {
        netClient->exitRoom();
        events->fireEvent(NAVIGATION_MAIN_MENU, transitionData());
    }

    void roomWaitingScreen::startRoomGame()
    {
        if (blockInputs)
        {
            return;
        }

        if (netClient->getRoomCount() <= 1)
        {
            popup.clearActions();
            popup.openWarningPopup("Not enough players available");
            return;
        }

        netGameManager->setupGame(netClient->getUpdatedRoom(), handSize, cardsPath, seed);
        netGameManager->startGame();
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

    void roomWaitingScreen::updateStatingCardsCount()
    {
        auto button = dynamic_cast<elements::card*>(rdr->getElement(buttons[0].id));
        std::stringstream ss;
        ss << "Number of starting cards [" << handSize << "]";
        button->setCenterText(ss.str());
    }

    void roomWaitingScreen::decreaseStartingCards()
    {
        handSize = max(4, handSize-1);
    }

    void roomWaitingScreen::increaseStartingCards()
    {
        handSize = min(12, handSize+1);
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
