#pragma once
#include "gameStateManager.h"
#include "../../framework.h"

class netGameStateManager : public gameStateManager
{
public:
    netGameStateManager(std::shared_ptr<eventBus::eventBus> events);
};
