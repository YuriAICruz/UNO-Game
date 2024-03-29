﻿#include "netGameStateManager.h"

#include <iostream>
#include <sstream>
#include <tuple>

#include "coreEventIds.h"
#include "netCommands.h"
#include "stringUtils.h"
#include "commands/client/drawCardsCmd.h"
#include "commands/client/endGameCmd.h"
#include "commands/client/getRoomCmd.h"
#include "commands/client/skipTurnCmd.h"
#include "commands/client/updateStateCmd.h"
#include "commands/server/lockRoomServerCmd.h"
#include "commands/server/drawCardsServerCmd.h"
#include "commands/server/endGameServerCmd.h"
#include "commands/server/executePlayerActionServerCmd.h"
#include "commands/client/gameSettingsCmd.h"
#include "commands/client/startGameCmd.h"
#include "commands/client/syncVarCmd.h"
#include "commands/client/unoYellCmd.h"
#include "commands/server/gameSettingsServerCmd.h"
#include "commands/server/skipTurnServerCmd.h"
#include "commands/server/startGameServerCmd.h"
#include "commands/server/syncVarServerCmd.h"
#include "commands/server/unoYellServerCmd.h"
#include "StateManager/gameEventData.h"

#define STATE_SYNC_DELAY 20

netGameStateManager::netGameStateManager(
    std::shared_ptr<eventBus::eventBus> events,
    std::shared_ptr<netcode::client> cl
) :
    gameStateManager(events), netClient(cl), isHost(false)
{
    addClientCommands();
}

netGameStateManager::netGameStateManager(
    std::shared_ptr<eventBus::eventBus> events,
    std::shared_ptr<netcode::client> cl,
    std::shared_ptr<netcode::server> sv
) :
    gameStateManager(events), netClient(cl),
    netServer(sv), isHost(true), isServer(true)
{
    addClientCommands();
    addServerCommands();
    sv->onClientReconnected = [this](netcode::clientInfo* client)
    {
        onClientReconnected(client);
    };
}

netGameStateManager::netGameStateManager(
    std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<netcode::server> sv
) :
    gameStateManager(events),
    netServer(sv), isHost(false), isServer(true)
{
    addServerCommands();
    sv->onClientReconnected = [this](netcode::clientInfo* client)
    {
        onClientReconnected(client);
    };
}

netGameStateManager::~netGameStateManager()
{
    if (netServer != nullptr)
    {
        netServer->onClientReconnected = nullptr;
    }
}

void netGameStateManager::setupGame(netcode::room* room, int handSize, std::string deckConfigFilePath, size_t seed)
{
    checkIsServer();
    gameStateManager::setupGame(room->getClientsNames(), room->getClientsIds(), handSize, deckConfigFilePath, seed);
    executeGameCommand<commands::gameSettingsCmd>(deckConfigFilePath);
}

void netGameStateManager::setupGame(
    std::vector<std::string>& players, int handSize, std::string deckConfigFilePath, size_t seed)
{
    checkIsServer();
    gameStateManager::setupGame(players, handSize, deckConfigFilePath, seed);
    executeGameCommand<commands::gameSettingsCmd>(deckConfigFilePath);
}

void netGameStateManager::setupGame(std::vector<std::string> players, std::vector<uint16_t> playersIds, int handSize,
                                    std::string deckConfigFilePath, size_t seed)
{
    checkIsServer();
    gameStateManager::setupGame(players, playersIds, handSize, deckConfigFilePath, seed);
    executeGameCommand<commands::gameSettingsCmd>(deckConfigFilePath);
}

std::string netGameStateManager::encryptGameSettings(const std::string& path, const std::string& cmdKey) const
{
    std::stringstream ss;

    int size = turner->playersCount();
    ss << cmdKey << NC_SEPARATOR;
    ss << static_cast<int>(handSize) << NC_SEPARATOR;
    ss << path << NC_SEPARATOR;
    ss << std::to_string(seed) << NC_SEPARATOR;
    ss << size << NC_SEPARATOR;
    int i = 0;
    for (uint16_t id : turner->getPlayersIds())
    {
        if (i > 0)
        {
            ss << NC_SEPARATOR;
        }
        ss << id;
        i++;
    }

    return ss.str();
}

void netGameStateManager::decryptGameSettingsAndSetup(const std::string& msg)
{
    decryptGameSettingsAndSetup(stringUtils::splitString(msg));
}

void netGameStateManager::decryptGameSettingsAndSetup(const std::vector<std::string>& data)
{
    int hand = std::stoi(data[1]);
    std::string path = data[2];
    size_t seed = std::stoull(data[3]);
    int size = std::stoi(data[4]);
    std::vector<uint16_t> ids;
    std::vector<std::string> names;
    ids.resize(size);
    names.resize(size);

    for (int i = 0; i < size; ++i)
    {
        ids[i] = std::stoi(data[5 + i]);

        if (!isServer || isHost)
        {
            netcode::room* room;
            netClient->executeCommand<commands::getRoomCmd>(room);
            names[i] = room->getClientName(ids[i]);
        }
        else
        {
            names[i] = std::to_string(ids[i]);
        }
    }

    gameStateManager::setupGame(names, ids, hand, path, seed);
}

void netGameStateManager::startGame()
{
    if (isServer)
    {
        baseStartGame();
        return;
    }
    if ((!isServer || isHost) && !running)
    {
        executeGameCommand<commands::startGameCmd>();
    }
}

void netGameStateManager::baseStartGame()
{
    gameStateManager::startGame();
}

bool netGameStateManager::isCurrentPlayer()
{
    if (isServer && !isHost)
    {
        return false;
    }
    return getCurrentPlayer()->Id() == netClient->getId();
}

turnSystem::IPlayer* netGameStateManager::getLocalPlayer() const
{
    return turner->getPlayerFromId(netClient->getId());
}

bool netGameStateManager::tryExecutePlayerAction(cards::ICard* card)
{
    if (isServer)
    {
        return gameStateManager::tryExecutePlayerAction(card);
    }

    throw std::exception("passing card unsupported, pass index instead");
}

void netGameStateManager::addServerCommands()
{
    addGameServerCallback<commands::drawCardsServerCmd>();
    addGameServerCallback<commands::executePlayerActionServerCmd>();
    addGameServerCallback<commands::gameSettingsServerCmd>();
    addGameServerCallback<commands::skipTurnServerCmd>();
    addGameServerCallback<commands::startGameServerCmd>();
    addGameServerCallback<commands::syncVarServerCmd>();
    addGameServerCallback<commands::unoYellServerCmd>();
}

void netGameStateManager::addClientCommands()
{
    addGameCallback<commands::endGameCmd>();
    addGameCallback<commands::gameSettingsCmd>("");
    addGameCallback<commands::startGameCmd>();
    addGameCallback<commands::unoYellCmd>();
    addGameCallback<commands::syncVarCmd>(0, 0);
    addGameCallback<commands::updateStateCmd>();
}

void netGameStateManager::checkIsServer() const
{
    if (isServer && !isHost)
    {
        throw std::exception("invalid action, server can't execute this");
    }
}

void netGameStateManager::checkIsClient() const
{
    if (!isServer)
    {
        throw std::exception("invalid action, client can't execute this");
    }
}

void netGameStateManager::broadcastServerStateData(SOCKET cs)
{
    if (!isServer)
    {
        return;
    }

    std::tuple<const char*, size_t> data = getState();
    size_t bufferSize =
        strlen(CORE_NC_UPDATE_STATE) * sizeof(char) +
        sizeof(char) + //NC_SEPARATOR
        sizeof(size_t) + //state Size
        sizeof(char) + //NC_SEPARATOR
        std::get<1>(data) +
        sizeof(char); //NC_COMMAND_END

    char* buffer = new char[bufferSize];
    char* ptr = buffer;

    encryptStateBuffer(data, ptr);

    netServer->broadcastToRoomRaw(buffer, bufferSize, cs);
    delete std::get<0>(data);
}

void netGameStateManager::sendToClientServerStateData(SOCKET cs)
{
    if (!isServer)
    {
        return;
    }

    std::tuple<const char*, size_t> data = getState();
    size_t bufferSize =
        strlen(CORE_NC_UPDATE_STATE) * sizeof(char) +
        sizeof(char) + //NC_SEPARATOR
        sizeof(size_t) + //state Size
        sizeof(char) + //NC_SEPARATOR
        std::get<1>(data) +
        sizeof(char); //NC_COMMAND_END

    char* buffer = new char[bufferSize];
    char* ptr = buffer;

    encryptStateBuffer(data, ptr);

    netServer->sendMessageRaw(cs, buffer, bufferSize, 0);
    delete std::get<0>(data);
}

void netGameStateManager::encryptStateBuffer(std::tuple<const char*, size_t> data, char* ptr)
{
    char separator = NC_SEPARATOR;
    char endCmd = NC_COMMAND_END;

    std::memcpy(ptr, &CORE_NC_UPDATE_STATE, strlen(CORE_NC_UPDATE_STATE) * sizeof(char));
    ptr += strlen(CORE_NC_UPDATE_STATE) * sizeof(char);
    std::memcpy(ptr, &separator, sizeof(char));
    ptr += sizeof(char);
    std::memcpy(ptr, &std::get<1>(data), sizeof(size_t));
    ptr += sizeof(size_t);
    std::memcpy(ptr, &separator, sizeof(char));
    ptr += sizeof(char);
    std::memcpy(ptr, std::get<0>(data), std::get<1>(data));
    ptr += std::get<1>(data);
    std::memcpy(ptr, &endCmd, sizeof(char));
}

void netGameStateManager::setStateNet(char* buffer, size_t size)
{
    char* ptr = buffer;
    ptr += strlen(CORE_NC_UPDATE_STATE) * sizeof(char);
    ptr += sizeof(char);

    size_t bufferSize;
    std::memcpy(&bufferSize, ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    ptr += sizeof(char);

    setState(ptr, bufferSize);
}

void netGameStateManager::cheatWin()
{
    if (!isServer)
    {
        throw std::exception("cheat not enabled in clients");
    }
    gameStateManager::cheatWin();
}

void netGameStateManager::baseEndGame()
{
    gameStateManager::endGame();
}

void netGameStateManager::endGame()
{
    if (!isServer)
    {
        running = false;
        events->fireEvent(GAME_END, gameEventData(getCurrentPlayer(), true));
        return;
    }

    executeGameServerCommand<commands::endGameServerCmd>();
}

bool netGameStateManager::skipTurn()
{
    if (!isServer)
    {
        return executeGameCommand<commands::skipTurnCmd>();
    }

    return gameStateManager::skipTurn();
}

void netGameStateManager::onClientReconnected(netcode::clientInfo* client)
{
    if (running)
    {
        sendToClientServerStateData(client->connection);
    }
}

bool netGameStateManager::baseYellUno()
{
    return gameStateManager::yellUno();
}

bool netGameStateManager::yellUno()
{
    if (!isServer)
    {
        return executeGameCommand<commands::unoYellCmd>();
    }

    return baseYellUno();
}

bool netGameStateManager::makePlayerDraw(turnSystem::IPlayer* player, int count)
{
    if (!isServer)
    {
        return executeGameCommand<commands::drawCardsCmd>(player->Id(), count);
    }

    return gameStateManager::makePlayerDraw(player, count);
}

void netGameStateManager::setRoom(netcode::room* room)
{
    serverRoom = room;
}

int netGameStateManager::getSyncVar(int id) const
{
    auto it = syncValuesDictionary.find(id);
    if (it != syncValuesDictionary.end())
    {
        return it->second;
    }

    return 0;
}

netcode::room* netGameStateManager::getServerRoom() const
{
    checkIsClient();
    return serverRoom;
}

bool netGameStateManager::isInRoom(SOCKET sc) const
{
    checkIsClient();
    return serverRoom->hasClientConnection(sc);
}

void netGameStateManager::updateVarsDictionary(int id, int value)
{
    syncValuesDictionary.insert_or_assign(id, value);
}

void netGameStateManager::roomGameStarted() const
{
    if (onRoomGameStarted != nullptr)
    {
        onRoomGameStarted();
    }
}

bool netGameStateManager::canYellUno() const
{
    if (isServer)
    {
        return gameStateManager::canYellUno();
    }

    return getCurrentPlayer()->getHand().size() == 2;
}
