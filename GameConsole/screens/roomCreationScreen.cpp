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
        buttons[0].action = [this]
        {
            createNewRoom();
        };

        lastX = winSize.X / 2 + offset;
        buttons[1].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            }, '+', 'g', "", "Select");
        buttons[1].action = [this]
        {
            listRooms();
        };

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

        buttons[currentButton].action();
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

    void roomCreationScreen::createNewRoom()
    {
        std::string roomName = "New Room";
        box.openStringEditBox("Enter Room Name", roomName, [this, roomName](const std::string& newValue)
        {
            netClient->createRoom(newValue);
            moveToNextRoom();
        });
    }

    void roomCreationScreen::listRooms()
    {
        blockInputs = true;
        rdr->blank();

        std::vector<netcode::room> rooms = netClient->getRooms();

        std::cout << "Rooms available on the server:" << "\n";
        int index = 0;
        for (auto room : rooms)
        {
            std::cout << index << ": " << room.getName();
            std::cout << ":" << room.getId();
            std::cout << "with (" << room.count() << ") players." << "\n";
            index++;
        }

        std::cout << "type the room index or [q] to exit.\n";
        while (true)
        {
            try
            {
                std::string strIndex;
                std::getline(std::cin, strIndex);
                if (strIndex == "q")
                {
                    clearRoomsList();
                    return;
                }
                index = std::stoul(strIndex);
                if (index < rooms.size() && index >= 0)
                {
                    break;
                }
                std::cerr << "ERROR: Invalid index" << std::endl;
            }
            catch (const std::invalid_argument& e)
            {
                std::cerr << "ERROR: Invalid argument: " << e.what() << std::endl;
            } catch (const std::out_of_range& e)
            {
                std::cerr << "ERROR: Out of range: " << e.what() << std::endl;
            }
        }

        netClient->enterRoom(rooms[index].getId());
        clearRoomsList();

        moveToNextRoom();
    }

    void roomCreationScreen::moveToNextRoom()
    {
        events->fireEvent(NAVIGATION_NETWORK_WAIT_ROOM, transitionData());
        hide();
    }

    void roomCreationScreen::clearRoomsList()
    {
        blockInputs = false;
        rdr->setDirty();
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
