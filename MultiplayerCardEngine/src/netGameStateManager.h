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
    std::promise<bool>* tryExecuteActionCallback = nullptr;
    std::promise<bool>* tryStartGameCallback = nullptr;
    std::promise<bool>* gameStateUpdatedCallback = nullptr;
    bool isHost = false;
    bool isServer = false;

public:
    netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<client> cl);
    void createServerCustomCommands();
    netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<client> cl, std::shared_ptr<server> sv);
    netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<server> sv);
    void startGame() override;
    void tryStartGame(const std::string& msg, SOCKET cs);
    void gameStartCallback(const std::string& msg);
    void createClientCustomCommands();
    bool isCurrentPlayer();
    bool tryExecutePlayerAction(cards::ICard* card) override;
    bool tryExecutePlayerAction(int index);
    void netPlayerActionCallback(const std::string& msg);
    void sendServerStateData(SOCKET cs);
    void tryExecuteNetPlayerAction(const std::string& msg, SOCKET cs);
    void setStateNet(char* buffer, size_t size);
};
