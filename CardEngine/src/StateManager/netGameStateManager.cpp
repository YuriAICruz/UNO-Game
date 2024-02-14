#include "pch.h"
#include "netGameStateManager.h"

netGameStateManager::netGameStateManager(std::shared_ptr<eventBus::eventBus> events)
    : gameStateManager(events)
{
}
