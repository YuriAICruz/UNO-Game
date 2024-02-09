#include "settingsMenu.h"

#include <sstream>

#include "../renderer/elements/card.h"

namespace screens
{
    void settingsMenu::show()
    {
        IScreen::show();

        rdr->clear();
        COORD winSize = rdr->getWindowSize();

        std::string title = "Settings";
        int buttonWidth = 40;
        int buttonHeight = 3;
        int offset = 1;

        int lastX = winSize.X / 2 - buttonWidth / 2;
        int lastY = 5;

        size_t titleId = rdr->addElement<elements::text>(
            COORD{
                static_cast<SHORT>(winSize.X / 2 - title.length() / 2),
                static_cast<SHORT>(lastY),
            },
            '+',
            'w',
            title
        );

        lastX = 10;
        lastY += offset + buttonHeight + offset;

        std::stringstream ss;
        ss << "Number of cards [" << handCount << "]";

        buttons[0].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            },
            '+',
            'g',
            "",
            ss.str()
        );
        buttons[0].actionLeft = [this]
        {
            handCount = max(2, handCount-1);

            std::stringstream ss;
            ss << "Number of cards [" << handCount << "]";
            auto cardsNumbersButton = static_cast<elements::card*>(rdr->getElement(buttons[0].id));
            cardsNumbersButton->setCenterText(ss.str());
            rdr->setDirty();
        };
        buttons[0].actionRight = [this]
        {
            handCount = min(12, handCount+1);

            std::stringstream ss;
            ss << "Number of cards [" << handCount << "]";
            auto cardsNumbersButton = static_cast<elements::card*>(rdr->getElement(buttons[0].id));
            cardsNumbersButton->setCenterText(ss.str());
            rdr->setDirty();
        };

        lastY += buttonHeight + offset;
        ss.str("");
        ss << "Deck configuration json path: \"" << configFilePath << "\"";
        buttons[1].id = rdr->addElement<elements::card>(
            COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY),
            }, COORD{
                static_cast<SHORT>(buttonWidth),
                static_cast<SHORT>(buttonHeight),
            },
            '+',
            'g',
            "",
            ss.str()
        );
        buttons[1].action = [this]
        {
            openStringEditBox("Deck configuration json path", configFilePath, [this]
            {
                std::stringstream ss;
                ss << "Deck configuration json path: \"" << configFilePath << "\"";
                auto filePathButton = static_cast<elements::card*>(rdr->getElement(buttons[1].id));
                filePathButton->setCenterText(ss.str());
                rdr->setDirty();
            });
        };

        selectButton(currentButton);
        rdr->setDirty();
    }

    void settingsMenu::moveUp(input::inputData data)
    {
        if (blockInputs)
        {
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

    void settingsMenu::moveDown(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }

        deselectButton(currentButton);
        currentButton = (currentButton + 1) % std::size(buttons);
        selectButton(currentButton);
        rdr->setDirty();
    }

    void settingsMenu::moveLeft(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        buttons[currentButton].actionLeft();
    }

    void settingsMenu::moveRight(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        buttons[currentButton].actionRight();
    }

    void settingsMenu::accept(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        buttons[currentButton].action();
    }

    void settingsMenu::cancel(input::inputData data)
    {
        if (blockInputs)
        {
            return;
        }
        events->fireEvent(NAVIGATION_MAIN_MENU, transitionData());
        hide();
    }

    void settingsMenu::selectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->select();
    }

    void settingsMenu::deselectButton(int index) const
    {
        auto button = static_cast<elements::card*>(rdr->getElement(buttons[index].id));
        button->deselect();
    }

    void settingsMenu::openStringEditBox(std::string title, std::string& data, const std::function<void()>& callback)
    {
        blockInputs = true;
        rdr->blank();
        std::string newValue;
        std::cout << "Enter a new value for " << title << ", you can cancel by typing 'quit'" << "\n";
        std::cout << "Current value is: " << data << "\n";
        std::getline(std::cin, newValue);
        if(newValue != "quit")
        {
            data = newValue;
        }
        callback();
        blockInputs = false;
    }
}
