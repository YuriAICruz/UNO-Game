#include "netGameStateManager.h"

netGameStateManager::netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<client> cl)
    : gameStateManager(events), netClient(cl), isHost(false)
{
}

netGameStateManager::netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<client> cl,
    std::shared_ptr<server> sv) : gameStateManager(events), netClient(cl), netServer(sv), isHost(true)
{
}
