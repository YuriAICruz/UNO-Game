#include <gtest/gtest.h>
#include <memory>

#include "logger.h"
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

    return sv;
}

std::shared_ptr<client> startClient()
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

    return cl;
}

std::unique_ptr<gameStateManager> createGameManager(int players = 2, int handInitialSize = 7)
{
    std::vector<std::string> playersList = std::vector<std::string>(players);
    for (int i = 0; i < players; ++i)
    {
        std::stringstream ss;
        ss << "Player 0" << i;
        playersList[i] = ss.str();
    }
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto manager = std::make_unique<gameStateManager>(events);
    manager->setupGame(playersList, handInitialSize, "Data\\deck_setup.json", 1234);
    manager->startGame();
    return manager;
}

TEST(NetGameFlowTests, Begin)
{
    auto sv = startServer();

    auto clA = startClient();
    auto clB = startClient();

    clA->createRoom("GameRoom");
    while (!clA->hasRoom() && !clA->hasError())
    {
    }

    clB->enterRoom(clA->getRoomId());
    while (!clB->hasRoom() && !clB->hasError())
    {
    }
}
