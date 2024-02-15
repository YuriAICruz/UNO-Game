#include "netGameStateManager.h"

#include <sstream>
#include <tuple>

#include "logger.h"
#include "netCommands.h"
#include "stringUtils.h"

netGameStateManager::netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<client> cl)
    : gameStateManager(events), netClient(cl), isHost(false)
{
    createClientCustomCommands();
}

netGameStateManager::netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<client> cl,
                                         std::shared_ptr<server> sv) : gameStateManager(events), netClient(cl),
                                                                       netServer(sv), isHost(true)
{
    createClientCustomCommands();
    std::map<std::string, std::function<void (std::string&, SOCKET)>> commands = {
        {
            CORE_NC_PLAYCARD, [this](std::string& msg, SOCKET cs)
            {
                tryExecuteNetPlayerAction(msg, cs);
            }
        }
    };

    sv->addCustomCommands(commands);
}

void netGameStateManager::createClientCustomCommands()
{
    std::map<std::string, std::function<void (std::string&)>> commands = {
        {
            CORE_NC_PLAYCARD, [this](std::string& msg)
            {
                netPlayerActionFailedCallback(msg);
            }
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

bool netGameStateManager::isCurrentPlayer()
{
    return getCurrentPlayer()->Id() == netClient->getId();
}

bool netGameStateManager::tryExecutePlayerAction(cards::ICard* card)
{
    throw std::exception("passing card unsupported, pass index instead");
}

bool netGameStateManager::tryExecutePlayerAction(int index)
{
    std::promise<bool> promise;
    actionCallback = &promise;
    auto future = promise.get_future();
    std::stringstream ss;
    ss << CORE_NC_PLAYCARD << NC_SEPARATOR << index;
    std::string str = ss.str();
    netClient->sendMessage(str.c_str());
    future.wait();
    return future.get();
}

void netGameStateManager::netPlayerActionFailedCallback(const std::string& msg)
{
    actionCallback->set_value(false);
    actionCallback = nullptr;
}

void netGameStateManager::tryExecuteNetPlayerAction(const std::string& msg, SOCKET cs)
{
    std::vector<std::string> data = stringUtils::splitString(msg);
    int index = std::stoi(data[1]);
    turnSystem::IPlayer* currentPlayer = getCurrentPlayer();
    if (gameStateManager::tryExecutePlayerAction(currentPlayer->pickCard(index)))
    {
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
    else
    {
        send(cs, CORE_NC_PLAYCARD, strlen(CORE_NC_PLAYCARD), 0);
    }
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

    if (actionCallback != nullptr)
    {
        actionCallback->set_value(true);
        actionCallback = nullptr;
    }
}
