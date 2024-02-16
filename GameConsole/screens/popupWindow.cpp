#include "popupWindow.h"

#include "../renderer/elements/card.h"

void popupWindow::show()
{
    COORD windowSize = rdr->getWindowSize();

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
}

void popupWindow::hide()
{
}

void popupWindow::hidePopup()
{
    isPopupOpen = false;
    auto popup = dynamic_cast<elements::card*>(rdr->getElement(popupButton.id));
    popup->setSize(COORD{0, 0});
    popup->setTitleText("");
    popup->setCenterText("");
    rdr->setDirty();
}

bool popupWindow::isOpen()
{
    return isPopupOpen;
}

void popupWindow::openWarningPopup(std::string bodyText)
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

void popupWindow::executeActionCancel()
{
    if (cancelAction != nullptr)
    {
        cancelAction();
        cancelAction = nullptr;
    }
}

void popupWindow::assignActionCancel(std::function<void()> action)
{
    cancelAction = action;
}


void popupWindow::executeActionAccept()
{
    if (acceptAction != nullptr)
    {
        acceptAction();
        acceptAction = nullptr;
    }
}

void popupWindow::assignActionAccept(std::function<void()> action)
{
    acceptAction = action;
}

void popupWindow::clearActions()
{
    acceptAction = nullptr;
    cancelAction = nullptr;
}
