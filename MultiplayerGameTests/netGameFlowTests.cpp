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

std::shared_ptr<netGameStateManager> createHostGameManager(std::shared_ptr<client> client,
                                                           std::shared_ptr<server> server,
                                                           int handInitialSize, int seed)
{
    auto room = server->getRoom(client->getRoomId());
    int players = room->count();
    std::vector<std::string> playersList = std::vector<std::string>(players);
    std::vector<size_t> playersIds = std::vector<size_t>(players);

    server->setSeed(seed);

    for (int i = 0; i < players; ++i)
    {
        playersList[i] = room->getClientByIndex(i)->name;
        playersIds[i] = room->getClientByIndex(i)->id;
    }
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto manager = std::make_shared<netGameStateManager>(events, client, server);
    manager->setupGame(playersList, playersIds, handInitialSize, "Data\\deck_setup.json", server->getSeed());
    manager->startGame();
    return manager;
}

std::shared_ptr<netGameStateManager> createClientGameManager(std::shared_ptr<client> client, int handInitialSize)
{
    auto r = client->getRoom();
    int seed = client->getSeed();
    int players = r->count();
    std::vector<std::string> playersList = std::vector<std::string>(players);
    std::vector<size_t> playersIds = std::vector<size_t>(players);

    for (int i = 0; i < players; ++i)
    {
        playersList[i] = r->getClientByIndex(i)->name;
        playersIds[i] = r->getClientByIndex(i)->id;
    }
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto manager = std::make_shared<netGameStateManager>(events, client);
    manager->setupGame(playersList, playersIds, handInitialSize, "Data\\deck_setup.json", seed);
    manager->startGame();
    return manager;
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
    auto hostManager = createHostGameManager(clA, sv, handSize, 12345);
    auto clientManager = createClientGameManager(clB, handSize);

    EXPECT_EQ(hostManager->getCurrentPlayer()->Id(), clientManager->getCurrentPlayer()->Id());

    EXPECT_TRUE(hostManager->isCurrentPlayer());
    EXPECT_FALSE(clientManager->isCurrentPlayer());

    EXPECT_EQ(*hostManager->getTopCard(), *clientManager->getTopCard());

    auto currentPlayer = hostManager->getCurrentPlayer();
    auto clientCurrentPlayer = clientManager->getCurrentPlayer();
    auto topCard = hostManager->getTopCard();
    auto hand = currentPlayer->getHand();
    int index = 0;
    int validIndex = 0;
    int invalidIndex = 0;
    for (auto card : hand)
    {
        if (card->sameColor(*topCard) || card->sameNumber(*topCard))
        {
            validIndex = index;
        }
        invalidIndex = index;
        index++;
    }

    EXPECT_FALSE(hostManager->tryExecutePlayerAction(invalidIndex));
    EXPECT_TRUE(hostManager->tryExecutePlayerAction(validIndex));
    
    EXPECT_NE(*currentPlayer, *hostManager->getCurrentPlayer());
    EXPECT_NE(*currentPlayer, *clientManager->getCurrentPlayer());
    EXPECT_LT(currentPlayer->getHand().size(), handSize);
    EXPECT_LT(clientCurrentPlayer->getHand().size(), handSize);

    closeClient(clA.get());
    closeClient(clB.get());
    closeServer(sv.get());
}
