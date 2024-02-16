#pragma once
#include <string>

#include "buttons.h"
#include "../renderer/renderer.h"

class popupWindow
{
    renderer::renderer* rdr;
    screens::button popupButton;
    bool isPopupOpen = false;
    std::function<void()> acceptAction;
    std::function<void()> cancelAction;

public:
    explicit popupWindow(renderer::renderer* render) : rdr(render)
    {
    }

    void show();
    void hide();
    void hidePopup();
    bool isOpen();
    void openWarningPopup(std::string bodyText);

    void executeActionCancel();
    void assignActionCancel(std::function<void()> action);

    void executeActionAccept();
    void assignActionAccept(std::function<void()> action);
    void clearActions();
};
