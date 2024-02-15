#pragma once
#include <memory>

#include "client/client.h"
#include "server/server.h"
#include "StateManager/gameStateManager.h"
#include "../framework.h"

class NET_ENGINE_API netGameStateManager : public gameStateManager
{
private:
    std::shared_ptr<client> netClient;
    std::shared_ptr<server> netServer;
    std::promise<bool>* actionCallback = nullptr;
    bool isHost = false;

public:
    netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<client> cl);
    netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<client> cl, std::shared_ptr<server> sv);
    void createClientCustomCommands();
    bool isCurrentPlayer();
    bool tryExecutePlayerAction(cards::ICard* card) override;
    bool tryExecutePlayerAction(int index);
    void netPlayerActionFailedCallback(const std::string& msg);
    void tryExecuteNetPlayerAction(const std::string& msg, SOCKET cs);
    void setStateNet(char* buffer, size_t size);
};
