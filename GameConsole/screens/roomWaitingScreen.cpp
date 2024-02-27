﻿#include "roomWaitingScreen.h"

#include <sstream>

#include "../renderer/elements/card.h"
#include "../renderer/elements/text.h"
#include "commands/client/exitRoomCmd.h"
#include "commands/client/setNameCmd.h"
#include "commands/client/getRoomCmd.h"
#include "commands/client/getUpdatedRoomCmd.h"
#include "commands/client/getSeedCmd.h"
#include "commands/client/setNotReadyCmd.h"
#include "commands/client/setReadyCmd.h"

namespace screens
{
    void roomWaitingScreen::show()
    {
        IScreen::show();
        rdr->clear();

        isRoomReady = false;

        netcode::room* room;
        netClient->executeCommand<commands::getRoomCmd>(room);
        std::stringstream ss;
        ss << "Room: " << room->getName() << ", waiting for players";
        std::string titleText = ss.str();

        COORD windowSize = rdr->getWindowSize();
        int lastX = windowSize.X / 2 - titleText.size() / 2;
        int lastY = 1;
        titleId = rdr->addElement<elements::text>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY)
            },
            ' ',
            'g',
            titleText
        );

        int buttonWidth = 45;
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
            "SetName:"
        );
        buttons[0].action = [this]
        {
            setPlayerName();
        };
        updatePlayerName();

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
            "Number of Starting Cards []"
        );
        buttons[1].actionLeft = [this]
        {
            decreaseStartingCards();
        };
        buttons[1].actionRight = [this]
        {
            increaseStartingCards();
        };
        updateStatingCardsCount();


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
            "Set Seed []"
        );
        buttons[2].action = [this]
        {
            setSeed();
        };
        updateSeed();


        lastY += buttonHeight + offset;
        buttons[3].id = rdr->addElement<elements::card>(
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
            "Set Ready"
        );
        buttons[3].action = [this]
        {
            toggleReady();
        };
        updateReadyState();

        lastY += buttonHeight + offset;
        buttons[4].id = rdr->addElement<elements::card>(
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
        buttons[4].action = [this]
        {
            startRoomGame();
        };

        lastY += buttonHeight + offset;
        buttons[5].id = rdr->addElement<elements::card>(
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
        buttons[5].action = [this]
        {
            tryExitRoom();
        };

        netcode::room* r;
        netClient->executeCommand<commands::getRoomCmd>(r);
        netClient->executeCommand<commands::getUpdatedRoomCmd>(r->getId());
        netClient->executeCommand<commands::getRoomCmd>(r);
        updateStartButton(r);

        netClient->onRoomUpdate = [this](netcode::room* r)
        {
            updateStartButton(r);
            updateReadyState();
        };
        netClient->onRoomReady = [this](bool ready)
        {
            updateStartButton(netGameManager->getRoom(), ready);
            updateReadyState();
        };
        netGameManager->onRoomGameStarted = [this]()
        {
            goToGameScreen();
        };

        popup.show();

        setButtonsSelectionColor();
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
        if (blockInputs || box.isBlocking())
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
        if (blockInputs || box.isBlocking())
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
        if (blockInputs || box.isBlocking())
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
        if (blockInputs || box.isBlocking())
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

        if (buttons[currentButton].action != nullptr)
        {
            buttons[currentButton].action();
        }
    }

    void roomWaitingScreen::cancel(input::inputData data)
    {
        if (blockInputs || box.isBlocking())
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

    void roomWaitingScreen::setSeed()
    {
        netClient->executeCommand<commands::getSeedCmd>(seed);
        box.openSizeTEditBox("Random Seed: ", seed, [this](std::string)
        {
            updateSeed();
        });
    }

    void roomWaitingScreen::updateSeed() const
    {
        std::stringstream ss;
        ss << "Random Seed: [" << seed << "]";
        elements::card* seedButton = dynamic_cast<elements::card*>(rdr->getElement(buttons[2].id));
        seedButton->setCenterText(ss.str());
        rdr->setDirty();
    }

    void roomWaitingScreen::toggleReady() const
    {
        netcode::room* room;
        netClient->executeCommand<commands::getRoomCmd>(room);

        if (room->isClientReady())
        {
            netClient->executeCommand<commands::setNotReadyCmd>(room);
        }
        else
        {
            netClient->executeCommand<commands::setReadyCmd>(room);
        }

        updateReadyState();
    }

    void roomWaitingScreen::updateReadyState() const
    {
        if (blockInputs || box.isBlocking())
        {
            return;
        }

        auto button = dynamic_cast<elements::card*>(rdr->getElement(buttons[3].id));

        netcode::room* room;
        netClient->executeCommand<commands::getRoomCmd>(room);

        std::stringstream ss;

        if (room->isClientReady())
        {
            ss << "Ready! [Cancel]";
        }
        else
        {
            ss << "Not Ready";
        }

        button->setCenterText(ss.str());
    }

    void roomWaitingScreen::updateStartButton(netcode::room* room, bool ready)
    {
        if (blockInputs || box.isBlocking())
        {
            return;
        }

        isRoomReady = ready;
        auto button = dynamic_cast<elements::card*>(rdr->getElement(buttons[4].id));

        std::stringstream ss;

        if (ready)
        {
            ss << "START - ";
        }
        else
        {
            ss << "WAITING - room not ready ";
        }

        ss << "[" << room->count() << "] players";
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

    void roomWaitingScreen::exitRoomAndReturnToMainScreen() const
    {
        netClient->executeCommand<commands::exitRoomCmd>();
        events->fireEvent(NAVIGATION_MAIN_MENU, transitionData());
    }

    void roomWaitingScreen::startRoomGame()
    {
        if (blockInputs || box.isBlocking())
        {
            return;
        }

        if (!isRoomReady)
        {
            popup.clearActions();
            popup.openWarningPopup("Room is not ready");
            return;
        }

        netcode::room* room;
        netClient->executeCommand<commands::getRoomCmd>(room);
        if (room->count() <= 1)
        {
            popup.clearActions();
            popup.openWarningPopup("Not enough players available");
            return;
        }

        blockInputs = true;

        netClient->executeCommand<commands::getUpdatedRoomCmd>(room->getId());
        netClient->executeCommand<commands::getRoomCmd>(room);

        netGameManager->setupGame(room, handSize, cardsPath, seed);
        events->fireEvent(NAVIGATION_ONLINE_GAME, transitionData());
    }

    void roomWaitingScreen::goToGameScreen()
    {
        if (blockInputs || box.isBlocking())
        {
            return;
        }

        events->fireEvent(NAVIGATION_ONLINE_GAME, transitionData());
        hide();
    }

    void roomWaitingScreen::updateStatingCardsCount()
    {
        auto button = dynamic_cast<elements::card*>(rdr->getElement(buttons[1].id));
        std::stringstream ss;
        ss << "Number of starting cards [" << handSize << "]";
        button->setCenterText(ss.str());
        rdr->setDirty();
    }

    void roomWaitingScreen::decreaseStartingCards()
    {
        handSize = max(4, handSize-1);

        updateStatingCardsCount();
    }

    void roomWaitingScreen::increaseStartingCards()
    {
        handSize = min(12, handSize+1);

        updateStatingCardsCount();
    }

    void roomWaitingScreen::updatePlayerName() const
    {
        auto button = dynamic_cast<elements::card*>(rdr->getElement(buttons[0].id));
        std::stringstream ss;
        ss << "SetPlayerName [" << netClient->getPlayerName() << "]";
        button->setCenterText(ss.str());
        rdr->setDirty();
    }

    void roomWaitingScreen::setPlayerName()
    {
        std::string name = netClient->getPlayerName();
        box.openStringEditBox("Player Name", name, [this](std::string newName)
        {
            netClient->executeCommand<commands::setNameCmd>(newName);
            updatePlayerName();
        });
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

    void roomWaitingScreen::setButtonsSelectionColor()
    {
        char color = 'b';
        for (auto button : buttons)
        {
            (dynamic_cast<elements::card*>(rdr->getElement(button.id)))->setSelectionColor(color);
        }
    }
}
