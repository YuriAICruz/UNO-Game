﻿#include "netGameStateManager.h"

#include <iostream>
#include <sstream>
#include <tuple>

#include "coreEventIds.h"
#include "logger.h"
#include "netCommands.h"
#include "stringUtils.h"
#include "StateManager/gameEventData.h"

netGameStateManager::netGameStateManager(
    std::shared_ptr<eventBus::eventBus> events,
    std::shared_ptr<netcode::client> cl
) :
    gameStateManager(events), netClient(cl), isHost(false)
{
    createClientCustomCommands();
}

netGameStateManager::netGameStateManager(
    std::shared_ptr<eventBus::eventBus> events,
    std::shared_ptr<netcode::client> cl,
    std::shared_ptr<netcode::server> sv
) :
    gameStateManager(events), netClient(cl),
    netServer(sv), isHost(true), isServer(true)
{
    createClientCustomCommands();
    createServerCustomCommands();

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
    createServerCustomCommands();

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

void netGameStateManager::setupGame(netcode::room* room, int handSize, std::string deckConfigFilePath,
                                    size_t seed)
{
    checkIsServer();
    gameStateManager::setupGame(room->getClientsNames(), room->getClientsIds(), handSize, deckConfigFilePath, seed);
    sendGameSettings(deckConfigFilePath);
}

void netGameStateManager::setupGame(std::vector<std::string>& players, int handSize, std::string deckConfigFilePath,
                                    size_t seed)
{
    checkIsServer();
    gameStateManager::setupGame(players, handSize, deckConfigFilePath, seed);
    sendGameSettings(deckConfigFilePath);
}

void netGameStateManager::setupGame(std::vector<std::string> players, std::vector<uint16_t> playersIds, int handSize,
                                    std::string deckConfigFilePath, size_t seed)
{
    checkIsServer();
    gameStateManager::setupGame(players, playersIds, handSize, deckConfigFilePath, seed);
    sendGameSettings(deckConfigFilePath);
}

void netGameStateManager::sendGameSettings(std::string path)
{
    if (!isServer && !running)
    {
        std::promise<bool> promise;
        trySetGameSettingsCallback = &promise;
        auto future = promise.get_future();

        std::string str = encryptGameSettings(path);
        netClient->sendMessage(str.c_str());
        future.wait();
        trySetGameSettingsCallback = nullptr;
    }
}

std::string netGameStateManager::encryptGameSettings(std::string path) const
{
    std::stringstream ss;
    int size = turner->playersCount();
    ss << CORE_NC_GAME_SETTINGS << NC_SEPARATOR;
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
    auto data = stringUtils::splitString(msg);

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
            auto room = netClient->getRoom();
            names[i] = room->getClientName(ids[i]);
        }
        else
        {
            names[i] = std::to_string(ids[i]);
        }
    }


    gameStateManager::setupGame(names, ids, hand, path, seed);
}

void netGameStateManager::trySetGameSettings(const std::string& msg, SOCKET cs)
{
    logger::print("SERVER: Updating Game Settings");
    decryptGameSettingsAndSetup(msg);
    logger::print("SERVER: Game Settings Updated");

    netServer->broadcastToRoom(msg, cs);
}

void netGameStateManager::gameSettingsCallback(const std::string& msg)
{
    decryptGameSettingsAndSetup(msg);

    if (trySetGameSettingsCallback != nullptr)
    {
        trySetGameSettingsCallback->set_value(true);
    }
}

void netGameStateManager::startGame()
{
    if (!isServer && !running)
    {
        std::promise<bool> promise;
        std::promise<bool> promiseState;
        tryStartGameCallback = &promise;
        gameStateUpdatedCallback = &promiseState;
        auto future = promise.get_future();
        auto stateFuture = promiseState.get_future();
        netClient->sendMessage(CORE_NC_GAME_START);
        future.wait();
        stateFuture.wait();
        gameStateUpdatedCallback = nullptr;
        tryStartGameCallback = nullptr;
    }
}

void netGameStateManager::tryStartGame(const std::string& msg, SOCKET cs)
{
    if (!isServer)
    {
        return;
    }
    if (!running)
    {
        gameStateManager::startGame();
    }
    netServer->lockRoom(cs);
    netServer->broadcastToRoom(CORE_NC_GAME_START, cs);
    broadcastServerStateData(cs);
}

void netGameStateManager::gameStartCallback(const std::string& msg)
{
    if (!running)
    {
        gameStateManager::startGame();
    }

    if (onRoomGameStarted != nullptr)
    {
        onRoomGameStarted();
    }
    if (tryStartGameCallback != nullptr)
    {
        tryStartGameCallback->set_value(true);
    }
}

void netGameStateManager::createClientCustomCommands()
{
    std::map<std::string, std::function<void (std::string&)>> commands = {
        {
            CORE_NC_PLAYCARD, [this](std::string& msg)
            {
                netPlayerActionCallback(msg);
            },
        },
        {
            CORE_NC_GAME_START, [this](std::string& msg)
            {
                gameStartCallback(msg);
            },
        },
        {
            CORE_NC_GAME_SETTINGS, [this](std::string& msg)
            {
                gameSettingsCallback(msg);
            },
        },
        {
            CORE_NC_SKIP_TURN, [this](std::string& msg)
            {
                skipTurnCallback(msg);
            },
        },
        {
            CORE_NC_YELL_UNO, [this](std::string& msg)
            {
                unoYellCallback(msg);
            },
        },
        {
            CORE_NC_DRAW_CARDS, [this](std::string& msg)
            {
                drawCardsCallback(msg);
            },
        },
        {
            CORE_NC_GAME_END, [this](std::string& msg)
            {
                showClientEndGame(msg);
            },
        }
    };
    std::map<std::string, std::function<void (char*, size_t)>> rawCommands = {
        {
            CORE_NC_UPDATE_STATE, [this](char* buffer, size_t size)
            {
                setStateNet(buffer, size);
            }
        }
    };

    netClient->addCustomCommands(commands);
    netClient->addCustomRawCommands(rawCommands);
}

void netGameStateManager::createServerCustomCommands()
{
    std::map<std::string, std::function<void (std::string&, SOCKET)>> commands = {
        {
            CORE_NC_PLAYCARD, [this](std::string& msg, SOCKET cs)
            {
                tryExecuteNetPlayerAction(msg, cs);
            }
        },
        {
            CORE_NC_GAME_START, [this](std::string& msg, SOCKET cs)
            {
                tryStartGame(msg, cs);
            }
        },
        {
            CORE_NC_GAME_SETTINGS, [this](std::string& msg, SOCKET cs)
            {
                trySetGameSettings(msg, cs);
            }
        },
        {
            CORE_NC_SKIP_TURN, [this](std::string& msg, SOCKET cs)
            {
                trySkipTurn(msg, cs);
            }
        },
        {
            CORE_NC_YELL_UNO, [this](std::string& msg, SOCKET cs)
            {
                tryYellUno(msg, cs);
            }
        },
        {
            CORE_NC_DRAW_CARDS, [this](std::string& msg, SOCKET cs)
            {
                tryDrawCards(msg, cs);
            }
        }
    };

    netServer->addCustomCommands(commands);
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
    throw std::exception("passing card unsupported, pass index instead");
}

void netGameStateManager::checkIsServer() const
{
    if (isServer && !isHost)
    {
        throw std::exception("invalid action, server can't execute this");
    }
}

bool netGameStateManager::tryExecutePlayerAction(int index)
{
    checkIsServer();

    if (!isCurrentPlayer())
    {
        return false;
    }

    if (tryExecuteActionCallback != nullptr)
    {
        tryExecuteActionCallback->get_future().wait();
    }
    std::promise<bool> promise;
    tryExecuteActionCallback = &promise;
    auto future = promise.get_future();
    std::stringstream ss;
    ss << CORE_NC_PLAYCARD << NC_SEPARATOR << index;
    std::string str = ss.str();
    netClient->sendMessage(str.c_str());
    future.wait();
    tryExecuteActionCallback = nullptr;
    return future.get();
}

void netGameStateManager::netPlayerActionCallback(const std::string& msg)
{
    std::vector<std::string> data = stringUtils::splitString(msg);
    if (tryExecuteActionCallback != nullptr)
    {
        int r = stoi(data[1]);
        tryExecuteActionCallback->set_value(r > 0);
    }
}

void netGameStateManager::broadcastServerStateData(SOCKET cs)
{
    if (!isServer)
    {
        return;
    }

    std::tuple<const char*, size_t> data = getState();
    size_t bufferSize = strlen(CORE_NC_UPDATE_STATE) * sizeof(char) +
        sizeof(char) +
        sizeof(size_t) +
        sizeof(char) +
        std::get<1>(data);

    char* buffer = new char[bufferSize];
    char* ptr = buffer;

    char separator = NC_SEPARATOR;

    std::cout << "\nSENT\n";
    print(std::get<0>(data), std::get<1>(data));
    std::cout << "\n----\n";

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

    netServer->broadcastToRoom(buffer, bufferSize, cs);
    delete std::get<0>(data);
}

void netGameStateManager::encryptStateBuffer(std::tuple<const char*, size_t> data, char* ptr)
{
    char separator = NC_SEPARATOR;

    std::cout << "\nSENT\n";
    print(std::get<0>(data), std::get<1>(data));
    std::cout << "\n----\n";

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
}

void netGameStateManager::sendToClientServerStateData(SOCKET cs)
{
    if (!isServer)
    {
        return;
    }

    std::tuple<const char*, size_t> data = getState();
    size_t bufferSize = strlen(CORE_NC_UPDATE_STATE) * sizeof(char) +
        sizeof(char) +
        sizeof(size_t) +
        sizeof(char) +
        std::get<1>(data);

    char* buffer = new char[bufferSize];
    char* ptr = buffer;

    encryptStateBuffer(data, ptr);

    netServer->sendMessage(cs, buffer, bufferSize, 0);
    delete std::get<0>(data);
}

void netGameStateManager::tryExecuteNetPlayerAction(const std::string& msg, SOCKET cs)
{
    std::vector<std::string> data = stringUtils::splitString(msg);
    int index = std::stoi(data[1]);
    turnSystem::IPlayer* currentPlayer = getCurrentPlayer();

    std::stringstream ss;
    ss << CORE_NC_PLAYCARD << NC_SEPARATOR;

    auto card = currentPlayer->pickCard(index);
    if (gameStateManager::tryExecutePlayerAction(card))
    {
        broadcastServerStateData(cs);
        ss << 1;
    }
    else
    {
        currentPlayer->receiveCard(card);
        ss << 0;
    }
    std::string str = ss.str();
    const char* response = str.c_str();
    send(cs, response, strlen(response), 0);
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

    if (gameStateUpdatedCallback != nullptr)
    {
        gameStateUpdatedCallback->set_value(true);
    }
}

void netGameStateManager::waitForStateSync()
{
    if (gameStateUpdatedCallback != nullptr)
    {
        gameStateUpdatedCallback->get_future().wait();
    }

    std::promise<bool> promise;
    gameStateUpdatedCallback = &promise;
    auto future = promise.get_future();
    future.wait();
    gameStateUpdatedCallback = nullptr;
}

void netGameStateManager::cheatWin()
{
    if (!isServer)
    {
        throw std::exception("cheat not enabled in clients");
    }
    gameStateManager::cheatWin();
}

void netGameStateManager::endGame()
{
    if (!isServer)
    {
        return;
    }

    gameStateManager::endGame();

    SOCKET sc = *serverRoom->clients()[0]->connection;

    if (!isInRoom(sc))
    {
        return;
    }
    netServer->broadcastToRoom(CORE_NC_GAME_END, sc);
    broadcastServerStateData(sc);
}

void netGameStateManager::onClientReconnected(netcode::clientInfo* client)
{
    if (running)
    {
        sendToClientServerStateData(*client->connection);
    }
}

void netGameStateManager::showClientEndGame(const std::string& msg)
{
    checkIsServer();

    running = false;
    events->fireEvent(GAME_END, gameEventData(getCurrentPlayer(), true));
}

bool netGameStateManager::skipTurn()
{
    checkIsServer();

    if (!isCurrentPlayer())
    {
        return false;
    }

    std::promise<bool> promise;
    std::promise<bool> promiseCmd;
    gameStateUpdatedCallback = &promise;
    executeCommandCallback = &promiseCmd;
    auto future = promise.get_future();
    auto futureCmd = promiseCmd.get_future();
    netClient->sendMessage(CORE_NC_SKIP_TURN);
    futureCmd.wait();
    future.wait();
    gameStateUpdatedCallback = nullptr;
    executeCommandCallback = nullptr;
    return futureCmd.get();
}

void netGameStateManager::trySkipTurn(const std::string& msg, SOCKET cs)
{
    if (!isServer)
    {
        return;
    }

    bool canSkip = canSkipTurn();
    if (canSkip)
    {
        gameStateManager::skipTurn();
    }

    std::stringstream ss;
    ss << CORE_NC_SKIP_TURN << NC_SEPARATOR << (canSkip ? 1 : 0);
    std::string str = ss.str();
    const char* resonse = str.c_str();
    send(cs, resonse, strlen(resonse), 0);

    broadcastServerStateData(cs);
}

void netGameStateManager::skipTurnCallback(const std::string& msg)
{
    commandCallbackResponse(msg);
}

bool netGameStateManager::yellUno()
{
    if (isServer)
    {
        gameStateManager::yellUno();
        return true;
    }

    if (!isCurrentPlayer())
    {
        return false;
    }

    std::promise<bool> promiseCmd;
    executeCommandCallback = &promiseCmd;
    auto futureCmd = promiseCmd.get_future();
    netClient->sendMessage(CORE_NC_YELL_UNO);
    futureCmd.wait();
    executeCommandCallback = nullptr;
    return futureCmd.get();
}

void netGameStateManager::tryYellUno(const std::string& msg, SOCKET cs)
{
    bool canYell = canYellUno();
    if (canYell)
    {
        gameStateManager::yellUno();
    }
    std::stringstream ss;
    ss << CORE_NC_YELL_UNO << NC_SEPARATOR << (canYell ? "1" : "0");
    std::string str = ss.str();
    netServer->broadcastToRoom(str, cs);
}

void netGameStateManager::unoYellCallback(const std::string& msg)
{
    auto data = stringUtils::splitString(msg);

    bool canYell = data[1] == "1";
    if (executeCommandCallback != nullptr)
    {
        executeCommandCallback->set_value(canYell);
    }

    if (canYell)
    {
        gameStateManager::yellUno();
    }
}

bool netGameStateManager::makePlayerDraw(turnSystem::IPlayer* player, int count)
{
    if (isServer)
    {
        gameStateManager::makePlayerDraw(player, count);
        return true;
    }

    if (!isCurrentPlayer())
    {
        return false;
    }

    std::promise<bool> promise;
    std::promise<bool> promiseCmd;

    gameStateUpdatedCallback = &promise;
    executeCommandCallback = &promiseCmd;

    auto future = promise.get_future();
    auto futureCmd = promiseCmd.get_future();

    std::stringstream ss;
    ss << CORE_NC_DRAW_CARDS << NC_SEPARATOR << player->Id() << NC_SEPARATOR << count;
    netClient->sendMessage(ss.str().c_str());

    futureCmd.wait();
    future.wait();

    gameStateUpdatedCallback = nullptr;
    executeCommandCallback = nullptr;

    return futureCmd.get();
}

void netGameStateManager::setRoom(netcode::room* room)
{
    serverRoom = room;
}

bool netGameStateManager::isInRoom(SOCKET sc) const
{
    if (!isServer)
    {
        return false;
    }

    return serverRoom->hasClientConnection(sc);
}

void netGameStateManager::tryDrawCards(const std::string& msg, SOCKET cs)
{
    auto data = stringUtils::splitString(msg);

    bool canDraw = canDrawCard();
    if (canDraw)
    {
        auto id = stoi(data[1]);
        auto amount = stoi(data[2]);

        gameStateManager::makePlayerDraw(getPlayerFromId(id), amount);
    }

    std::stringstream ss;
    ss << CORE_NC_SKIP_TURN << NC_SEPARATOR << (canDraw ? 1 : 0);
    std::string str = ss.str();
    const char* resonse = str.c_str();
    send(cs, resonse, strlen(resonse), 0);

    broadcastServerStateData(cs);
}

void netGameStateManager::drawCardsCallback(const std::string& msg) const
{
    commandCallbackResponse(msg);
}

void netGameStateManager::commandCallbackResponse(const std::string& msg) const
{
    if (executeCommandCallback != nullptr)
    {
        auto data = stringUtils::splitString(msg);
        executeCommandCallback->set_value(data[1] == "1");
    }
}
