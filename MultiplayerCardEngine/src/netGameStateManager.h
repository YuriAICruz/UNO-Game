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
    std::promise<bool>* executeCommandCallback = nullptr;
    bool isHost = false;
    bool isServer = false;
    netcode::room* serverRoom;

public:
    std::function<void()> onRoomGameStarted;

public:
    netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<netcode::client> cl);
    netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<netcode::client> cl,
                        std::shared_ptr<netcode::server> sv);
    netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<netcode::server> sv);

    ~netGameStateManager() override;
    
    void createServerCustomCommands();

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
    std::string encryptGameSettings(std::string path) const;

    void sendGameSettings(std::string path);
    void decryptGameSettingsAndSetup(const std::string& msg);
    void trySetGameSettings(const std::string& msg, SOCKET cs);
    void gameSettingsCallback(const std::string& msg);
    void startGame() override;
    void tryStartGame(const std::string& msg, SOCKET cs);
    void gameStartCallback(const std::string& msg);
    void createClientCustomCommands();
    bool isCurrentPlayer();
    turnSystem::IPlayer* getLocalPlayer() const;
    bool tryExecutePlayerAction(cards::ICard* card) override;
    void checkIsServer() const;
    bool tryExecutePlayerAction(int index);
    void netPlayerActionCallback(const std::string& msg) const;
    void broadcastServerStateData(SOCKET cs);
    void encryptStateBuffer(std::tuple<const char*, size_t> data, char* ptr);
    void sendToClientServerStateData(SOCKET cs);
    void tryExecuteNetPlayerAction(const std::string& msg, SOCKET cs);
    void setStateNet(char* buffer, size_t size);
    void waitForStateSync();
    void cheatWin() override;
    void endGame() override;
    bool skipTurn() override;
    bool yellUno() override;
    bool makePlayerDraw(turnSystem::IPlayer* player, int count) override;
    void setRoom(netcode::room* room);
    netcode::room* getRoom() const;

private:
    bool isInRoom(SOCKET sc) const;
    void onClientReconnected(netcode::clientInfo* client);
    void showClientEndGame(const std::string& msg);

    void trySkipTurn(const std::string& msg, SOCKET cs);
    void commandCallbackResponse(const std::string& msg) const;
    void skipTurnCallback(const std::string& msg);

    void tryYellUno(const std::string& msg, SOCKET cs);
    void unoYellCallback(const std::string& msg);

    void tryDrawCards(const std::string& msg, SOCKET cs);
    void drawCardsCallback(const std::string& msg) const;
};
