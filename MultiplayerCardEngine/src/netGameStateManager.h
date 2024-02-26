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
    std::map<int, int> syncValuesDictionary;
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

    std::string encryptGameSettings(const std::string& path, const std::string& cmdKey) const;
    void decryptGameSettingsAndSetup(const std::string& msg);
    void decryptGameSettingsAndSetup(const std::vector<std::string>& data);

    void startGame() override;
    void createClientCustomCommands();
    bool isCurrentPlayer();
    turnSystem::IPlayer* getLocalPlayer() const;
    bool tryExecutePlayerAction(cards::ICard* card) override;
    void broadcastServerStateData(SOCKET cs);
    void encryptStateBuffer(std::tuple<const char*, size_t> data, char* ptr);
    void sendToClientServerStateData(SOCKET cs);
    void setStateNet(char* buffer, size_t size);
    void waitForStateSync();
    void cheatWin() override;
    void endGame() override;
    bool skipTurn() override;
    bool yellUno() override;
    bool makePlayerDraw(turnSystem::IPlayer* player, int count) override;
    void setRoom(netcode::room* room);
    netcode::room* getRoom() const;
    void setSyncVar(int id, int value);
    int getSyncVar(int id) const;
    void trySyncVar(const std::string& msg, SOCKET cs);
    void syncVarCallback(const std::string& msg);
    bool canYellUno() const override;

    template <typename T, typename... Args>
    bool NETCODE_API executeGameCommand(Args&&... args)
    {
        checkIsServer();
        return netClient->executeCommand<T>(std::forward<Args>(args)..., this);
    }

    template <typename T, typename... Args>
    bool NETCODE_API executeGameServerCommand(Args&&... args)
    {
        checkIsClient();
        return netServer->executeServerCommand<T>(std::forward<Args>(args)..., this);
    }

private:
    void checkIsServer() const;
    void checkIsClient() const;

    bool isInRoom(SOCKET sc) const;
    void onClientReconnected(netcode::clientInfo* client);
    void showClientEndGame(const std::string& msg);
};
