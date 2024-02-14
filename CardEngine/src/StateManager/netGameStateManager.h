#pragma once
#include "gameStateManager.h"
#include "../../framework.h"

class ENGINE_API netGameStateManager : public gameStateManager
{
public:
    netGameStateManager(std::shared_ptr<eventBus::eventBus> events);
};
