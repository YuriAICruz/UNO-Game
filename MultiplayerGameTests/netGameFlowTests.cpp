#include <gtest/gtest.h>
#include <memory>

#include "logger.h"
#include "netCommands.h"
#include "client/client.h"
#include "server/server.h"
#include "StateManager/gameStateManager.h"
#include "StateManager/netGameStateManager.h"

std::shared_ptr<server> startServer()
{
    auto sv = std::make_shared<server>();

    sv->start();
    while (!sv->isRunning() && !sv->hasError())
    {
    }
    EXPECT_TRUE(sv->isRunning());

    std::map<std::string, std::function<void (std::string&, SOCKET)>> commands = {
        {
            CORE_NC_PLAYCARD, [](std::string& msg, SOCKET cs)
            {
            }
        }
    };
    sv->addCustomCommands(commands);

    return sv;
}

std::shared_ptr<client> startClient(std::string name)
{
    logger::print("TEST: startAndConnectClient");
    auto cl = std::make_shared<client>();

    cl->start();
    EXPECT_TRUE(cl->isRunning());

    cl->connectToServer();
    while (!cl->isConnected() && !cl->hasError())
    {
    }
    EXPECT_TRUE(cl->isConnected());

    cl->setName(name);

    return cl;
}

std::unique_ptr<gameStateManager> createGameManager(room* r, int handInitialSize = 7)
{
    int players = r->count();
    std::vector<std::string> playersList = std::vector<std::string>(players);

    for (int i = 0; i < players; ++i)
    {
        playersList[i] = r->getClientByIndex(i)->name;
    }
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto manager = std::make_unique<netGameStateManager>(events);
    manager->setupGame(playersList, handInitialSize, "Data\\deck_setup.json", 1234);
    manager->startGame();
    return manager;
}

TEST(NetGameFlowTests, Begin)
{
    auto sv = startServer();

    auto clA = startClient("Player A");
    auto clB = startClient("Player B");

    clA->createRoom("GameRoom");
    while (!clA->hasRoom() && !clA->hasError())
    {
    }

    clB->enterRoom(clA->getRoomId());
    while (!clB->hasRoom() && !clB->hasError())
    {
    }

    int handSize = 7;
    auto manager = createGameManager(sv->getRoom(clA->getRoomId()), handSize);

    for (int i = 0, players = sv->getRoom(clA->getRoomId())->count(); i < players; ++i)
    {
        EXPECT_EQ(handSize, manager->getPlayer(i)->getHand().size());
    }
}
