#include <future>
#include <gtest/gtest.h>
#include <memory>

#include "logger.h"
#include "netCommands.h"
#include "netGameStateManager.h"
#include "stringUtils.h"
#include "Cards/ActionTypes/base.h"
#include "client/client.h"
#include "server/server.h"
#include "StateManager/gameStateManager.h"

#define STATE_SYNC_DELAY 200
bool waiting;

std::shared_ptr<netcode::server> startServer()
{
    auto sv = std::make_shared<netcode::server>();

    sv->start();
    while (!sv->isRunning() && !sv->hasError())
    {
    }
    EXPECT_TRUE(sv->isRunning());

    return sv;
}

void closeServer(netcode::server* sv)
{
    logger::print("TEST: closing Server");
    sv->close();
    while (sv->isRunning())
    {
    }
    EXPECT_FALSE(sv->isRunning());
    std::this_thread::sleep_for(std::chrono::milliseconds(STATE_SYNC_DELAY));
}

void closeClient(netcode::client* cl)
{
    logger::print("TEST: closeClient");
    cl->close();
    while (cl->isConnected() || cl->isRunning())
    {
    }
    EXPECT_FALSE(cl->isConnected());
    EXPECT_FALSE(cl->isRunning());
}

std::shared_ptr<netcode::client> startClient(std::string name)
{
    logger::print("TEST: startAndConnectClient");
    auto cl = std::make_shared<netcode::client>();

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

void createRoom(std::string roomName, std::shared_ptr<netcode::client> client)
{
    client->createRoom(roomName);
    while (!client->hasRoom() && !client->hasError())
    {
    }
}

void joinRoom(int roomId, std::shared_ptr<netcode::client> clB)
{
    clB->enterRoom(roomId);
    while (!clB->hasRoom() && !clB->hasError())
    {
    }
}

std::shared_ptr<gameStateManager> createGameManager(netcode::room* r, int handInitialSize = 7)
{
    int players = r->count();
    std::vector<std::string> playersList = std::vector<std::string>(players);

    for (int i = 0; i < players; ++i)
    {
        playersList[i] = r->getClientByIndex(i)->name;
    }
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto manager = std::make_shared<gameStateManager>(events);
    manager->bindGameEvents();
    manager->setupGame(playersList, handInitialSize, "Data\\deck_setup.json", 12345);
    manager->startGame();
    return manager;
}

std::shared_ptr<netGameStateManager> createHostGameManager(std::shared_ptr<netcode::client> client,
                                                           std::shared_ptr<netcode::server> server,
                                                           int handInitialSize, int seed)
{
    auto room = server->getRoom(client->getRoomId());
    int players = room->count();
    std::vector<std::string> playersList = std::vector<std::string>(players);
    std::vector<uint16_t> playersIds = std::vector<uint16_t>(players);

    server->setSeed(seed);

    for (int i = 0; i < players; ++i)
    {
        playersList[i] = room->getClientByIndex(i)->name;
        playersIds[i] = room->getClientByIndex(i)->id;
    }
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto manager = std::make_shared<netGameStateManager>(events, client, server);
    manager->bindGameEvents();
    manager->setupGame(playersList, playersIds, handInitialSize, "Data\\deck_setup.json", server->getSeed());
    manager->startGame();
    return manager;
}

std::shared_ptr<netGameStateManager> createServerGameManager(netcode::room* room,
                                                             std::shared_ptr<netcode::server> server,
                                                             int handInitialSize, int seed)
{
    int players = room->count();
    std::vector<std::string> playersList = std::vector<std::string>(players);
    std::vector<uint16_t> playersIds = std::vector<uint16_t>(players);

    server->setSeed(seed);

    for (int i = 0; i < players; ++i)
    {
        playersList[i] = room->getClientByIndex(i)->name;
        playersIds[i] = room->getClientByIndex(i)->id;
    }
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto manager = std::make_shared<netGameStateManager>(events, server);
    manager->bindGameEvents();
    //skipping setup and game start, those will be called from client
    return manager;
}

std::shared_ptr<netGameStateManager> createClientGameManager(std::shared_ptr<netcode::client> client,
                                                             int handInitialSize)
{
    auto r = client->getUpdatedRoom();
    int seed = client->getSeed();
    int players = r->count();
    std::vector<std::string> playersList = std::vector<std::string>(players);
    std::vector<uint16_t> playersIds = std::vector<uint16_t>(players);

    for (int i = 0; i < players; ++i)
    {
        playersList[i] = r->getClientByIndex(i)->name;
        playersIds[i] = r->getClientByIndex(i)->id;
    }
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto manager = std::make_shared<netGameStateManager>(events, client);
    manager->bindGameEvents();
    manager->setupGame(playersList, playersIds, handInitialSize, "Data\\deck_setup.json", seed);
    manager->startGame();
    return manager;
}

TEST(NetGameFlowTests, Begin)
{
    auto sv = startServer();
    ASSERT_TRUE(sv->isRunning());

    std::shared_ptr<netcode::client> clA = startClient("Player A");
    std::shared_ptr<netcode::client> clB = startClient("Player B");

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
    ASSERT_TRUE(sv->isRunning());

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
    ASSERT_TRUE(sv->isRunning());

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
        if (
            card->sameColor(*topCard) ||
            card->sameNumber(*topCard) ||
            (!card->actionType()->isEqual(typeid(cards::actions::base)) && card->sameType(*topCard))
        )
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

TEST(NetGameFlowTests, PlayCardFromManagerDedicatedServer)
{
    auto sv = startServer();
    ASSERT_TRUE(sv->isRunning());

    auto clA = startClient("Player A");
    auto clB = startClient("Player B");

    createRoom("GameRoom", clA);
    joinRoom(clA->getRoomId(), clB);

    int handSize = 7;
    auto serverManager = createServerGameManager(clA->getUpdatedRoom(), sv, handSize, 12345);
    auto clientManagerA = createClientGameManager(clA, handSize);
    auto clientManagerB = createClientGameManager(clB, handSize);

    EXPECT_EQ(serverManager->getCurrentPlayer()->Id(), clientManagerA->getCurrentPlayer()->Id());
    EXPECT_EQ(serverManager->getCurrentPlayer()->Id(), clientManagerB->getCurrentPlayer()->Id());

    EXPECT_FALSE(serverManager->isCurrentPlayer());
    EXPECT_TRUE(clientManagerA->isCurrentPlayer());
    EXPECT_FALSE(clientManagerB->isCurrentPlayer());

    EXPECT_EQ(*serverManager->getTopCard(), *clientManagerA->getTopCard());
    EXPECT_EQ(*serverManager->getTopCard(), *clientManagerB->getTopCard());

    auto currentPlayer = serverManager->getCurrentPlayer();
    auto clientCurrentPlayerA = clientManagerA->getCurrentPlayer();
    auto clientCurrentPlayerB = clientManagerB->getCurrentPlayer();
    auto topCard = serverManager->getTopCard();
    auto hand = currentPlayer->getHand();
    int index = 0;
    int validIndex = 0;
    int invalidIndex = 0;
    for (auto card : hand)
    {
        if (
            card->sameColor(*topCard) ||
            card->sameNumber(*topCard) ||
            (!card->actionType()->isEqual(typeid(cards::actions::base)) && card->sameType(*topCard))
        )
        {
            validIndex = index;
        }
        invalidIndex = index;
        index++;
    }

    EXPECT_FALSE(clientManagerA->tryExecutePlayerAction(invalidIndex));
    EXPECT_FALSE(clientManagerB->tryExecutePlayerAction(validIndex));
    EXPECT_TRUE(clientManagerA->tryExecutePlayerAction(validIndex));

    EXPECT_NE(*currentPlayer, *serverManager->getCurrentPlayer());
    EXPECT_NE(*currentPlayer, *clientManagerA->getCurrentPlayer());
    EXPECT_NE(*currentPlayer, *clientManagerB->getCurrentPlayer());
    EXPECT_LT(currentPlayer->getHand().size(), handSize);
    EXPECT_LT(clientCurrentPlayerA->getHand().size(), handSize);
    EXPECT_LT(clientCurrentPlayerB->getHand().size(), handSize);

    closeClient(clA.get());
    closeClient(clB.get());
    closeServer(sv.get());
}

TEST(NetGameFlowTests, StartSessionWithDedicatedServer)
{
    auto sv = startServer();
    ASSERT_TRUE(sv->isRunning());

    auto clA = startClient("Player A");
    auto clB = startClient("Player B");

    clA->createRoom("TestRoom");
    clB->enterRoom(clA->getRoomId());

    int handSize = 7;
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto serverManager = std::make_shared<netGameStateManager>(events, sv);
    auto clientManagerA = std::make_shared<netGameStateManager>(events, clA);
    auto clientManagerB = std::make_shared<netGameStateManager>(events, clB);
    serverManager->bindGameEvents();

    clientManagerA->setupGame(clA->getRoom(), handSize, "Data\\deck_setup.json", 12345);

    clientManagerA->startGame();

    EXPECT_EQ(serverManager->getCurrentPlayer()->Id(), clientManagerA->getCurrentPlayer()->Id());
    EXPECT_EQ(serverManager->getCurrentPlayer()->Id(), clientManagerB->getCurrentPlayer()->Id());

    EXPECT_FALSE(serverManager->isCurrentPlayer());
    EXPECT_TRUE(clientManagerA->isCurrentPlayer());
    EXPECT_FALSE(clientManagerB->isCurrentPlayer());

    auto stc = serverManager->getTopCard();
    auto ctc = clientManagerA->getTopCard();
    EXPECT_EQ(*serverManager->getTopCard(), *clientManagerA->getTopCard());
    EXPECT_EQ(*serverManager->getTopCard(), *clientManagerB->getTopCard());

    auto currentPlayer = serverManager->getCurrentPlayer();
    auto clientCurrentPlayerA = clientManagerA->getCurrentPlayer();
    auto clientCurrentPlayerB = clientManagerB->getCurrentPlayer();
    auto topCard = serverManager->getTopCard();
    auto hand = currentPlayer->getHand();
    int index = 0;
    int validIndex = 0;
    int invalidIndex = 0;
    for (auto card : hand)
    {
        if (
            card->sameColor(*topCard) ||
            card->sameNumber(*topCard) ||
            (!card->actionType()->isEqual(typeid(cards::actions::base)) && card->sameType(*topCard))
        )
        {
            validIndex = index;
        }
        invalidIndex = index;
        index++;
    }

    EXPECT_FALSE(clientManagerA->tryExecutePlayerAction(invalidIndex));
    EXPECT_FALSE(clientManagerB->tryExecutePlayerAction(validIndex));
    EXPECT_TRUE(clientManagerA->tryExecutePlayerAction(validIndex));

    EXPECT_NE(*currentPlayer, *serverManager->getCurrentPlayer());
    EXPECT_NE(*currentPlayer, *clientManagerA->getCurrentPlayer());
    EXPECT_NE(*currentPlayer, *clientManagerB->getCurrentPlayer());
    EXPECT_LT(currentPlayer->getHand().size(), handSize);
    EXPECT_LT(clientCurrentPlayerA->getHand().size(), handSize);
    EXPECT_LT(clientCurrentPlayerB->getHand().size(), handSize);

    closeClient(clA.get());
    closeClient(clB.get());
    closeServer(sv.get());
}

TEST(NetGameFlowTests, EnteringOnLockedRoom)
{
    auto sv = startServer();
    ASSERT_TRUE(sv->isRunning());

    auto clA = startClient("Player A");
    auto clB = startClient("Player B");
    auto clC = startClient("Player C");

    clA->createRoom("TestRoom");
    clB->enterRoom(clA->getRoomId());

    int handSize = 7;
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto serverManager = std::make_shared<netGameStateManager>(events, sv);
    auto clientManagerA = std::make_shared<netGameStateManager>(events, clA);
    auto clientManagerB = std::make_shared<netGameStateManager>(events, clB);
    serverManager->bindGameEvents();

    clientManagerA->setupGame(clA->getRoom(), handSize, "Data\\deck_setup.json", 12345);

    clientManagerA->startGame();

    clC->enterRoom(clA->getRoomId());

    EXPECT_NE(clC->getRoom()->getName(), clA->getRoom()->getName());

    closeClient(clA.get());
    closeClient(clB.get());
    closeServer(sv.get());
}

TEST(NetGameFlowTests, ReconnectToRunningGame)
{
    auto sv = startServer();
    ASSERT_TRUE(sv->isRunning());

    auto clA = startClient("Player A");
    auto clB = startClient("Player B");

    clA->createRoom("TestRoom");
    clB->enterRoom(clA->getRoomId());

    int handSize = 7;
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto serverManager = std::make_shared<netGameStateManager>(events, sv);
    auto clientManagerA = std::make_shared<netGameStateManager>(events, clA);
    auto clientManagerB = std::make_shared<netGameStateManager>(events, clB);
    serverManager->bindGameEvents();

    clientManagerA->setupGame(clA->getRoom(), handSize, "Data\\deck_setup.json", 12345);

    auto startingPlayer = serverManager->getCurrentPlayer();
    clientManagerA->startGame();

    clB->close();

    EXPECT_TRUE(clientManagerA->makePlayerDraw(clientManagerA->getCurrentPlayer(), 1));
    EXPECT_TRUE(clientManagerA->skipTurn());

    clB->start();
    clB->connectToServer();
    clientManagerB->waitForStateSync();

    EXPECT_NE(serverManager->getCurrentPlayer()->Id(), startingPlayer->Id());
    EXPECT_EQ(serverManager->getCurrentPlayer()->Id(), clientManagerB->getCurrentPlayer()->Id());
    
    EXPECT_TRUE(clientManagerB->makePlayerDraw(clientManagerB->getCurrentPlayer(), 1));
    EXPECT_TRUE(clientManagerB->skipTurn());

    closeClient(clA.get());
    closeClient(clB.get());
    closeServer(sv.get());
}

TEST(NetGameFlowTests, CallUno)
{
    auto sv = startServer();
    ASSERT_TRUE(sv->isRunning());

    auto clA = startClient("Player A");
    auto clB = startClient("Player B");

    clA->createRoom("TestRoom");
    clB->enterRoom(clA->getRoomId());

    int handSize = 7;
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto serverManager = std::make_shared<netGameStateManager>(events, sv);
    auto clientManagerA = std::make_shared<netGameStateManager>(events, clA);
    auto clientManagerB = std::make_shared<netGameStateManager>(events, clB);
    serverManager->bindGameEvents();

    clientManagerA->setupGame(clA->getRoom(), handSize, "Data\\deck_setup.json", 12345);
    clientManagerA->startGame();
    EXPECT_FALSE(clientManagerA->yellUno());

    closeClient(clA.get());
    closeClient(clB.get());
    closeServer(sv.get());
}
