#include "netGameStateManager.h"

#include <sstream>

#include "netCommands.h"
#include "stringUtils.h"

netGameStateManager::netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<client> cl)
    : gameStateManager(events), netClient(cl), isHost(false)
{
}

netGameStateManager::netGameStateManager(std::shared_ptr<eventBus::eventBus> events, std::shared_ptr<client> cl,
                                         std::shared_ptr<server> sv) : gameStateManager(events), netClient(cl),
                                                                       netServer(sv), isHost(true)
{
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

void netGameStateManager::tryExecuteNetPlayerAction(const std::string& msg, SOCKET cs)
{
    std::vector<std::string> data = stringUtils::splitString(msg);
    int index = std::stoi(data[1]);
    turnSystem::IPlayer* currentPlayer = getCurrentPlayer();
    actionCallback->set_value(gameStateManager::tryExecutePlayerAction(currentPlayer->pickCard(index)));
    actionCallback = nullptr;
}
