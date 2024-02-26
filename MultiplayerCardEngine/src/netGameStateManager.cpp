#include "netGameStateManager.h"

#include <iostream>
#include <sstream>
#include <tuple>

#include "coreEventIds.h"
#include "logger.h"
#include "netCommands.h"
#include "stringUtils.h"
#include "commands/server/lockRoomServerCmd.h"
#include "StateManager/gameEventData.h"

#define STATE_SYNC_DELAY 20

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

void netGameStateManager::trySetGameSettings(const std::string& msg, SOCKET cs)
{
    logger::print("SERVER: Updating Game Settings");
    decryptGameSettingsAndSetup(msg);
    logger::print("SERVER: Game Settings Updated");

    netServer->broadcastUpdatedRoom(cs);
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
}

void netGameStateManager::createClientCustomCommands()
{
    std::map<std::string, std::function<void (std::string&)>> commands = {
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
        },
        {
            NC_SYNC_VAR, [this](std::string& msg)
            {
                syncVarCallback(msg);
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
        },
        {
            NC_SYNC_VAR, [this](std::string& msg, SOCKET cs)
            {
                trySyncVar(msg, cs);
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
    if (isServer)
    {
        return gameStateManager::tryExecutePlayerAction(card);
    }

    throw std::exception("passing card unsupported, pass index instead");
}

void netGameStateManager::checkIsServer() const
{
    if (isServer && !isHost)
    {
        throw std::exception("invalid action, server can't execute this");
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

    std::this_thread::sleep_for(std::chrono::milliseconds(STATE_SYNC_DELAY));
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

    std::this_thread::sleep_for(std::chrono::milliseconds(STATE_SYNC_DELAY));
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

    serverRoom->unlock();
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

    std::promise<bool> promiseCmd;
    executeCommandCallback = &promiseCmd;
    auto futureCmd = promiseCmd.get_future();
    netClient->sendMessage(CORE_NC_SKIP_TURN);
    futureCmd.wait();
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
    bool canYell = getCurrentPlayer()->getHand().size() == 2;
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

    std::promise<bool> promiseCmd;

    executeCommandCallback = &promiseCmd;

    auto futureCmd = promiseCmd.get_future();

    std::stringstream ss;
    ss << CORE_NC_DRAW_CARDS << NC_SEPARATOR << player->Id() << NC_SEPARATOR << count;
    std::string str = ss.str();
    netClient->sendMessage(str.c_str());

    futureCmd.wait();

    executeCommandCallback = nullptr;

    return futureCmd.get();
}

void netGameStateManager::setRoom(netcode::room* room)
{
    serverRoom = room;
}

netcode::room* netGameStateManager::getRoom() const
{
    if (isServer)
    {
        return serverRoom;
    }

    netcode::room* room;
    netClient->executeCommand<commands::getRoomCmd>(room);
    return room;
}

void netGameStateManager::setSyncVar(int id, int value)
{
    checkIsServer();
    syncValuesDictionary.insert_or_assign(id, value);

    std::stringstream ss;
    ss << NC_SYNC_VAR << NC_SEPARATOR << id << NC_SEPARATOR << value;
    std::string str = ss.str();
    netClient->sendMessage(str.c_str());
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

void netGameStateManager::trySyncVar(const std::string& msg, SOCKET cs)
{
    if (!isServer)
    {
        return;
    }

    auto data = stringUtils::splitString(msg);
    int id = stoi(data[1]);
    int value = stoi(data[2]);
    syncValuesDictionary.insert_or_assign(id, value);

    std::stringstream ss;
    ss << NC_SYNC_VAR << NC_SEPARATOR << id << NC_SEPARATOR << value;
    netServer->broadcastToRoom(ss.str(), cs);
}

void netGameStateManager::syncVarCallback(const std::string& msg)
{
    auto data = stringUtils::splitString(msg);
    int id = stoi(data[1]);
    int value = stoi(data[2]);
    syncValuesDictionary.insert_or_assign(id, value);
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
    logger::print(
        (logger::getPrinter() << "player draw cards : " << (canDraw ? "true" : "false") << " drawn = [" <<
            currentPlayerCardsDraw << "]").
        str());
    if (canDraw)
    {
        auto id = stoi(data[1]);
        auto amount = stoi(data[2]);

        gameStateManager::makePlayerDraw(getPlayerFromId(id), amount);
    }

    std::stringstream ss;
    ss << CORE_NC_DRAW_CARDS << NC_SEPARATOR << (canDraw ? 1 : 0);
    std::string str = ss.str();
    const char* resonse = str.c_str();
    send(cs, resonse, strlen(resonse), 0);

    broadcastServerStateData(cs);
}

void netGameStateManager::drawCardsCallback(const std::string& msg) const
{
    commandCallbackResponse(msg);
}

bool netGameStateManager::canYellUno() const
{
    if (isServer)
    {
        return gameStateManager::canYellUno();
    }

    return getCurrentPlayer()->getHand().size() == 2;
}

void netGameStateManager::commandCallbackResponse(const std::string& msg) const
{
    if (executeCommandCallback != nullptr)
    {
        auto data = stringUtils::splitString(msg);
        executeCommandCallback->set_value(data[1] == "1");
    }
}
