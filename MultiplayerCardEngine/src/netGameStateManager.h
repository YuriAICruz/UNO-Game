#pragma once
#include <memory>

#include "client/client.h"
#include "server/server.h"
#include "StateManager/gameStateManager.h"
#include "../framework.h"

class NET_ENGINE_API netGameStateManager : public gameStateManager
{
private:
    std::shared_ptr<netcode::client> netClient;
    std::shared_ptr<netcode::server> netServer;
    std::promise<bool>* tryExecuteActionCallback = nullptr;
    std::promise<bool>* tryStartGameCallback = nullptr;
    std::promise<bool>* trySetGameSettingsCallback = nullptr;
    std::promise<bool>* gameStateUpdatedCallback = nullptr;
    bool isHost = false;
    bool isServer = false;

public:
    std::function<void()> onRoomGameStarted;

public:
    netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<netcode::client> cl);
    void createServerCustomCommands();
    netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<netcode::client> cl,
                        std::shared_ptr<netcode::server> sv);
    netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<netcode::server> sv);

    void setupGame(
        netcode::room* room,
        int handSize,
        std::string deckConfigFilePath,
        size_t seed);

    void setupGame(
        std::vector<std::string>& players,
        int handSize,
        std::string deckConfigFilePath,
        size_t seed) override;

    void setupGame(
        std::vector<std::string> players,
        std::vector<uint16_t> playersIds,
        int handSize,
        std::string deckConfigFilePath,
        size_t seed) override;

    void sendGameSettings(std::string path);
    void decryptGameSettingsAndSetup(const std::string& msg);
    void trySetGameSettings(const std::string& msg, SOCKET cs);
    void gameSettingsCallback(const std::string& msg);
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
