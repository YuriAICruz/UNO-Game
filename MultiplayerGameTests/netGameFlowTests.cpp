#include <future>
#include <gtest/gtest.h>
#include <memory>

#include "logger.h"
#include "netCommands.h"
#include "netGameStateManager.h"
#include "stringUtils.h"
#include "client/client.h"
#include "server/server.h"
#include "StateManager/gameStateManager.h"

bool waiting;

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

void closeServer(server* sv)
{
    logger::print("TEST: closing Server");
    sv->close();
    while (sv->isRunning())
    {
    }
    EXPECT_FALSE(sv->isRunning());
}

void closeClient(client* cl)
{
    logger::print("TEST: closeClient");
    cl->close();
    while (cl->isConnected() || cl->isRunning())
    {
    }
    EXPECT_FALSE(cl->isConnected());
    EXPECT_FALSE(cl->isRunning());
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

void createRoom(std::string roomName, std::shared_ptr<client> client)
{
    client->createRoom(roomName);
    while (!client->hasRoom() && !client->hasError())
    {
    }
}

void joinRoom(int roomId, std::shared_ptr<client> clB)
{
    clB->enterRoom(roomId);
    while (!clB->hasRoom() && !clB->hasError())
    {
    }
}

std::shared_ptr<gameStateManager> createGameManager(room* r, int handInitialSize = 7)
{
    int players = r->count();
    std::vector<std::string> playersList = std::vector<std::string>(players);

    for (int i = 0; i < players; ++i)
    {
        playersList[i] = r->getClientByIndex(i)->name;
    }
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto manager = std::make_shared<gameStateManager>(events);
    manager->setupGame(playersList, handInitialSize, "Data\\deck_setup.json", 12345);
    manager->startGame();
    return manager;
}

std::shared_ptr<gameStateManager> createHostGameManager(std::shared_ptr<client> client, std::shared_ptr<server> server,
                                                        int handInitialSize = 7)
{
    auto room = server->getRoom(client->getRoomId());
    int players = room->count();
    std::vector<std::string> playersList = std::vector<std::string>(players);

    for (int i = 0; i < players; ++i)
    {
        playersList[i] = room->getClientByIndex(i)->name;
    }
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto manager = std::make_shared<netGameStateManager>(events, client, server);
    manager->setupGame(playersList, handInitialSize, "Data\\deck_setup.json", 12345);
    manager->startGame();
    return manager;
}

std::shared_ptr<gameStateManager> createClientGameManager(std::shared_ptr<client> client,int handInitialSize)
{
    std::promise<std::shared_ptr<gameStateManager>> promise;
    auto future = promise.get_future();
    client->updateRoom([client, handInitialSize, &promise](room* r)
    {
        int players = r->count();
        std::vector<std::string> playersList = std::vector<std::string>(players);

        for (int i = 0; i < players; ++i)
        {
            playersList[i] = r->getClientByIndex(i)->name;
        }
        std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
        auto manager = std::make_shared<netGameStateManager>(events, client);
        manager->setupGame(playersList, handInitialSize, "Data\\deck_setup.json", 12345);
        manager->startGame();
        promise.set_value(manager);
    });
    future.wait();
    return future.get();
}

TEST(NetGameFlowTests, Begin)
{
    auto sv = startServer();

    std::shared_ptr<client> clA = startClient("Player A");
    std::shared_ptr<client> clB = startClient("Player B");

    createRoom("GameRoom", clA);
    joinRoom(clA->getRoomId(), clB);

    int handSize = 7;
    auto manager = createGameManager(sv->getRoom(clA->getRoomId()), handSize);

    for (int i = 0, players = sv->getRoom(clA->getRoomId())->count(); i < players; ++i)
    {
        EXPECT_EQ(handSize, manager->getPlayer(i)->getHand().size());
    }

    closeClient(clA.get());
    closeClient(clB.get());
    closeServer(sv.get());
}

TEST(NetGameFlowTests, PlayRightCard)
{
    auto sv = startServer();

    auto clA = startClient("Player A");
    auto clB = startClient("Player B");

    createRoom("GameRoom", clA);
    joinRoom(clA->getRoomId(), clB);

    int handSize = 7;
    auto manager = createGameManager(sv->getRoom(clA->getRoomId()), handSize);

    std::map<std::string, std::function<void (std::string&, SOCKET)>> commands = {
        {
            CORE_NC_PLAYCARD, [manager](std::string& msg, SOCKET cs)
            {
                std::vector<std::string> data = stringUtils::splitString(msg);
                int index = std::stoi(data[1]);
                turnSystem::IPlayer* currentPlayer = manager->getCurrentPlayer();
                manager->tryExecutePlayerAction(currentPlayer->pickCard(index));
                waiting = false;
            }
        }
    };
    sv->addCustomCommands(commands);

    turnSystem::IPlayer* firstPlayer = manager->getCurrentPlayer();
    turnSystem::IPlayer* currentPlayer = manager->getCurrentPlayer();
    cards::ICard* topCard = manager->getTopCard();
    std::list<cards::ICard*> playerCards = currentPlayer->getHand();

    int index = 0;
    for (auto card : playerCards)
    {
        if (card->sameColor(*topCard) || card->sameNumber(*topCard))
        {
            break;
        }
        index++;
    }

    std::stringstream ss;
    ss << CORE_NC_PLAYCARD << NC_SEPARATOR << index;
    std::string str = ss.str();
    waiting = true;
    clA->sendMessage(str.c_str());
    while (waiting)
    {
    }

    EXPECT_NE(currentPlayer, manager->getCurrentPlayer());
    EXPECT_LT(currentPlayer->getHand().size(), handSize);

    currentPlayer = manager->getCurrentPlayer();
    topCard = manager->getTopCard();
    playerCards = currentPlayer->getHand();

    index = 0;
    for (auto card : playerCards)
    {
        if (card->sameColor(*topCard) || card->sameNumber(*topCard))
        {
            break;
        }
        index++;
    }

    ss.str("");
    ss << CORE_NC_PLAYCARD << NC_SEPARATOR << index;
    str = ss.str();
    waiting = true;
    clA->sendMessage(str.c_str());
    while (waiting)
    {
    }

    EXPECT_NE(currentPlayer, manager->getCurrentPlayer());
    EXPECT_EQ(firstPlayer, manager->getCurrentPlayer());
    EXPECT_LT(currentPlayer->getHand().size(), handSize);

    closeClient(clA.get());
    closeClient(clB.get());
    closeServer(sv.get());
}

TEST(NetGameFlowTests, PlayCardFromManager)
{
    auto sv = startServer();

    auto clA = startClient("Player A");
    auto clB = startClient("Player B");

    createRoom("GameRoom", clA);
    joinRoom(clA->getRoomId(), clB);

    int handSize = 7;
    auto hostManager = createHostGameManager(clA, sv, handSize);
    auto clientManager = createClientGameManager(clB, handSize);
}
