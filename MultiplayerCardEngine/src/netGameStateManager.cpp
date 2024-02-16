#include "netGameStateManager.h"

#include <atomic>
#include <sstream>
#include <tuple>

#include "logger.h"
#include "netCommands.h"
#include "stringUtils.h"

netGameStateManager::netGameStateManager(
    std::shared_ptr<eventBus::eventBus> events,
    std::shared_ptr<client> cl
) :
    gameStateManager(events), netClient(cl), isHost(false)
{
    createClientCustomCommands();
}

netGameStateManager::netGameStateManager(
    std::shared_ptr<eventBus::eventBus> events,
    std::shared_ptr<client> cl,
    std::shared_ptr<server> sv
) :
    gameStateManager(events), netClient(cl),
    netServer(sv), isHost(true), isServer(true)
{
    createClientCustomCommands();
    createServerCustomCommands();
}

netGameStateManager::netGameStateManager(
    std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<server> sv
) :
    gameStateManager(events),
    netServer(sv), isHost(false), isServer(true)
{
    createServerCustomCommands();
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
    netServer->broadcastToRoom(CORE_NC_GAME_START, cs);
    sendServerStateData(cs);
}

void netGameStateManager::gameStartCallback(const std::string& msg)
{
    if (!running)
    {
        gameStateManager::startGame();
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

bool netGameStateManager::tryExecutePlayerAction(cards::ICard* card)
{
    throw std::exception("passing card unsupported, pass index instead");
}

bool netGameStateManager::tryExecutePlayerAction(int index)
{
    if (isServer && !isHost)
    {
        throw std::exception("invalid action, pure server can't execute this");
    }

    if(!isCurrentPlayer())
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

void netGameStateManager::sendServerStateData(SOCKET cs)
{
    if (!isServer)
    {
        return;
    }
    auto data = getState();
    size_t bufferSize =
        strlen(CORE_NC_UPDATE_STATE) * sizeof(char) +
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

void netGameStateManager::tryExecuteNetPlayerAction(const std::string& msg, SOCKET cs)
{
    std::vector<std::string> data = stringUtils::splitString(msg);
    int index = std::stoi(data[1]);
    turnSystem::IPlayer* currentPlayer = getCurrentPlayer();

    std::stringstream ss;
    ss << CORE_NC_PLAYCARD << NC_SEPARATOR;

    if (gameStateManager::tryExecutePlayerAction(currentPlayer->pickCard(index)))
    {
        sendServerStateData(cs);
        ss << 1;
    }
    else
    {
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

    if (netClient->getId() == 1)
    {
        std::cout << "\nRECEIVED\n";
        print(ptr, bufferSize);
        std::cout << "\n----\n";
    }

    setState(ptr, bufferSize);

    if (gameStateUpdatedCallback != nullptr)
    {
        gameStateUpdatedCallback->set_value(true);
    }
}
