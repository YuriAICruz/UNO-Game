﻿#include <future>
#include <gtest/gtest.h>
#include <memory>

#include "gameVarIds.h"
#include "logger.h"
#include "netCommands.h"
#include "netGameStateManager.h"
#include "stringUtils.h"
#include "Cards/ActionTypes/base.h"
#include "client/client.h"
#include "commands/client/executePlayerActionCmd.h"
#include "commands/client/setNotReadyCmd.h"
#include "commands/client/setReadyCmd.h"
#include "commands/client/syncVarCmd.h"
#include "commands/client/setNameCmd.h"
#include "commands/client/hasRoomCmd.h"
#include "commands/client/getRoomCmd.h"
#include "commands/client/getUpdatedRoomCmd.h"
#include "commands/client/enterRoomCmd.h"
#include "commands/client/createRoomCmd.h"
#include "commands/client/getSeedCmd.h"
#include "server/server.h"
#include "StateManager/gameStateManager.h"

#define STATE_SYNC_DELAY 200
bool waiting;

std::shared_ptr<netcode::server> startServer()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(STATE_SYNC_DELAY));
    auto sv = std::make_shared<netcode::server>();

    sv->start();
    while (!sv->isRunning() && !sv->hasError())
    {
    }
    EXPECT_TRUE(sv->isRunning());
    EXPECT_FALSE(sv->hasError());

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

    cl->executeCommand<commands::setNameCmd>(name);

    return cl;
}

void createRoom(const std::string& roomName, std::shared_ptr<netcode::client> client)
{
    client->executeCommand<commands::createRoomCmd>(roomName);
    while (!client->executeCommand<commands::hasRoomCmd>() && !client->hasError())
    {
    }
}

void joinRoom(int roomId, std::shared_ptr<netcode::client> clB)
{
    clB->executeCommand<commands::enterRoomCmd>(roomId);
    while (!clB->executeCommand<commands::hasRoomCmd>() && !clB->hasError())
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
    netcode::room* room;
    client->executeCommand<commands::getRoomCmd>(room);
    client->executeCommand<commands::getUpdatedRoomCmd>(room->getId());
    client->executeCommand<commands::getRoomCmd>(room);
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

std::shared_ptr<netGameStateManager> createClientGameManager(
    std::shared_ptr<netcode::client> client, int handInitialSize)
{
    netcode::room* r;
    client->executeCommand<commands::getRoomCmd>(r);
    size_t seed;
    client->executeCommand<commands::getSeedCmd>(seed);

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
    //skipping game start, must be called directly from one client after all are retup
    return manager;
}

TEST(NetGameFlowTests, Begin)
{
    auto sv = startServer();
    ASSERT_TRUE(sv->isRunning());

    std::shared_ptr<netcode::client> clA = startClient("Player A");
    std::shared_ptr<netcode::client> clB = startClient("Player B");

    createRoom("GameRoom", clA);

    netcode::room* roomA;
    clA->executeCommand<commands::getRoomCmd>(roomA);

    joinRoom(roomA->getId(), clB);

    int handSize = 7;
    clA->executeCommand<commands::getUpdatedRoomCmd>(roomA->getId());
    auto manager = createGameManager(roomA, handSize);

    for (int i = 0, players = roomA->count(); i < players; ++i)
    {
        EXPECT_EQ(handSize, manager->getPlayer(i)->getHand().size());
    }

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

    netcode::room* roomA;
    clA->executeCommand<commands::getRoomCmd>(roomA);
    joinRoom(roomA->getId(), clB);

    int handSize = 7;
    auto hostManager = createHostGameManager(clA, sv, handSize, 12345);
    auto clientManager = createClientGameManager(clB, handSize);
    
    clientManager->startGame();

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

    EXPECT_FALSE(hostManager->executeGameCommand<commands::executePlayerActionCmd>(invalidIndex));
    EXPECT_TRUE(hostManager->executeGameCommand<commands::executePlayerActionCmd>(validIndex));

    EXPECT_NE(currentPlayer->Id(), hostManager->getCurrentPlayer()->Id());
    EXPECT_NE(currentPlayer->Id(), clientManager->getCurrentPlayer()->Id());
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
    netcode::room* roomA;
    clA->executeCommand<commands::getRoomCmd>(roomA);
    joinRoom(roomA->getId(), clB);

    int handSize = 7;
    clA->executeCommand<commands::getUpdatedRoomCmd>(roomA->getId());
    auto serverManager = createServerGameManager(roomA, sv, handSize, 12345);
    auto clientManagerA = createClientGameManager(clA, handSize);
    auto clientManagerB = createClientGameManager(clB, handSize);

    clientManagerA->startGame();
    std::this_thread::sleep_for(std::chrono::milliseconds(STATE_SYNC_DELAY));
    
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

    EXPECT_FALSE(clientManagerA->executeGameCommand<commands::executePlayerActionCmd>(invalidIndex));
    EXPECT_FALSE(clientManagerB->executeGameCommand<commands::executePlayerActionCmd>(validIndex));
    EXPECT_TRUE(clientManagerA->executeGameCommand<commands::executePlayerActionCmd>(validIndex));

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

    clA->executeCommand<commands::createRoomCmd>("TestRoom");
    netcode::room* roomA;
    clA->executeCommand<commands::getRoomCmd>(roomA);
    clB->executeCommand<commands::enterRoomCmd>(roomA->getId());

    int handSize = 7;
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto serverManager = std::make_unique<netGameStateManager>(events, sv);
    auto clientManagerA = std::make_unique<netGameStateManager>(events, clA);
    auto clientManagerB = std::make_unique<netGameStateManager>(events, clB);
    serverManager->bindGameEvents();

    clA->executeCommand<commands::getUpdatedRoomCmd>(roomA->getId());
    clientManagerA->setupGame(roomA, handSize, "Data\\deck_setup.json", 12345);

    clientManagerA->startGame();

    EXPECT_EQ(serverManager->getCurrentPlayer()->Id(), clientManagerA->getCurrentPlayer()->Id());
    EXPECT_EQ(serverManager->getCurrentPlayer()->Id(), clientManagerB->getCurrentPlayer()->Id());

    EXPECT_FALSE(serverManager->isCurrentPlayer());
    EXPECT_TRUE(clientManagerA->isCurrentPlayer());
    EXPECT_FALSE(clientManagerB->isCurrentPlayer());

    // delay to wait the clientManagerB to be sync with the server data 
    std::this_thread::sleep_for(std::chrono::milliseconds(STATE_SYNC_DELAY * 2));
    auto stc = serverManager->getTopCard();
    auto catc = clientManagerA->getTopCard();
    auto cbtc = clientManagerB->getTopCard();
    EXPECT_EQ(*stc, *catc);
    EXPECT_EQ(*stc, *cbtc);

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

    EXPECT_FALSE(clientManagerA->executeGameCommand<commands::executePlayerActionCmd>(invalidIndex));
    EXPECT_FALSE(clientManagerB->executeGameCommand<commands::executePlayerActionCmd>(validIndex));
    EXPECT_TRUE(clientManagerA->executeGameCommand<commands::executePlayerActionCmd>(validIndex));

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

    clA->executeCommand<commands::createRoomCmd>("TestRoom");
    netcode::room* roomA;
    clA->executeCommand<commands::getRoomCmd>(roomA);
    clB->executeCommand<commands::enterRoomCmd>(roomA->getId());

    int handSize = 7;
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto serverManager = std::make_shared<netGameStateManager>(events, sv);
    auto clientManagerA = std::make_shared<netGameStateManager>(events, clA);
    auto clientManagerB = std::make_shared<netGameStateManager>(events, clB);
    serverManager->bindGameEvents();

    clA->executeCommand<commands::getUpdatedRoomCmd>(roomA->getId());
    clientManagerA->setupGame(roomA, handSize, "Data\\deck_setup.json", 12345);

    clientManagerA->startGame();

    clC->executeCommand<commands::enterRoomCmd>(roomA->getId());
    netcode::room* roomC;
    clC->executeCommand<commands::getRoomCmd>(roomC);

    EXPECT_NE(roomC->getName(), roomA->getName());

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

    clA->executeCommand<commands::createRoomCmd>("TestRoom");
    netcode::room* roomA;
    clA->executeCommand<commands::getRoomCmd>(roomA);
    clB->executeCommand<commands::enterRoomCmd>(roomA->getId());

    int handSize = 7;
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto serverManager = std::make_shared<netGameStateManager>(events, sv);
    auto clientManagerA = std::make_shared<netGameStateManager>(events, clA);
    auto clientManagerB = std::make_shared<netGameStateManager>(events, clB);
    serverManager->bindGameEvents();

    clA->executeCommand<commands::getUpdatedRoomCmd>(roomA->getId());
    clientManagerA->setupGame(roomA, handSize, "Data\\deck_setup.json", 12345);

    auto startingPlayer = serverManager->getCurrentPlayer();
    clientManagerA->startGame();

    clB->close();

    EXPECT_TRUE(clientManagerA->makePlayerDraw(clientManagerA->getCurrentPlayer(), 1));
    EXPECT_TRUE(clientManagerA->skipTurn());

    clB->start();
    clB->connectToServer();

    //wait a little for the server data to be received by the client.
    std::this_thread::sleep_for(std::chrono::milliseconds(STATE_SYNC_DELAY));
    
    EXPECT_NE(serverManager->getCurrentPlayer()->Id(), startingPlayer->Id());
    EXPECT_EQ(serverManager->getCurrentPlayer()->Id(), clientManagerB->getCurrentPlayer()->Id());

    ASSERT_FALSE(clientManagerA->isCurrentPlayer());
    ASSERT_TRUE(clientManagerB->isCurrentPlayer());
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

    clA->executeCommand<commands::createRoomCmd>("TestRoom");
    netcode::room* roomA;
    clA->executeCommand<commands::getRoomCmd>(roomA);
    clB->executeCommand<commands::enterRoomCmd>(roomA->getId());

    int handSize = 7;
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto serverManager = std::make_shared<netGameStateManager>(events, sv);
    auto clientManagerA = std::make_shared<netGameStateManager>(events, clA);
    auto clientManagerB = std::make_shared<netGameStateManager>(events, clB);
    serverManager->bindGameEvents();

    clA->executeCommand<commands::getUpdatedRoomCmd>(roomA->getId());
    clientManagerA->setupGame(roomA, handSize, "Data\\deck_setup.json", 12345);
    clientManagerA->startGame();
    EXPECT_FALSE(clientManagerA->yellUno());

    closeClient(clA.get());
    closeClient(clB.get());
    closeServer(sv.get());
}

TEST(NetGameFlowTests, SyncVar)
{
    auto sv = startServer();
    ASSERT_TRUE(sv->isRunning());

    auto clA = startClient("Player A");
    auto clB = startClient("Player B");

    clA->executeCommand<commands::createRoomCmd>("TestRoom");
    netcode::room* roomA;
    clA->executeCommand<commands::getRoomCmd>(roomA);
    clB->executeCommand<commands::enterRoomCmd>(roomA->getId());

    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto serverManager = std::make_shared<netGameStateManager>(events, sv);
    auto clientManagerA = std::make_shared<netGameStateManager>(events, clA);
    auto clientManagerB = std::make_shared<netGameStateManager>(events, clB);
    serverManager->bindGameEvents();

    int initialHandSize = 8;
    int handSize = clientManagerA->getSyncVar(CG_VAR_HAND_SIZE);

    clientManagerA->executeGameCommand<commands::syncVarCmd>(CG_VAR_HAND_SIZE, initialHandSize);
    handSize = clientManagerA->getSyncVar(CG_VAR_HAND_SIZE);
    EXPECT_EQ(initialHandSize, handSize);
    handSize = clientManagerB->getSyncVar(CG_VAR_HAND_SIZE);
    while (handSize == 0)
    {
        handSize = clientManagerB->getSyncVar(CG_VAR_HAND_SIZE);
    }
    EXPECT_EQ(initialHandSize, handSize);

    initialHandSize = 7;
    clientManagerB->executeGameCommand<commands::syncVarCmd>(CG_VAR_HAND_SIZE, initialHandSize);
    handSize = 12;
    handSize = clientManagerB->getSyncVar(CG_VAR_HAND_SIZE);
    EXPECT_EQ(initialHandSize, handSize);
    while (handSize != clientManagerA->getSyncVar(CG_VAR_HAND_SIZE))
    {
    }
    handSize = clientManagerA->getSyncVar(CG_VAR_HAND_SIZE);
    EXPECT_EQ(initialHandSize, handSize);

    clA->executeCommand<commands::getUpdatedRoomCmd>(roomA->getId());
    clientManagerA->setupGame(roomA, handSize, "Data\\deck_setup.json", 12345);
    clientManagerA->startGame();

    closeClient(clA.get());
    closeClient(clB.get());
    closeServer(sv.get());
}

TEST(NetGameFlowTests, RoomReady)
{
    auto sv = startServer();
    ASSERT_TRUE(sv->isRunning());

    auto clA = startClient("Player A");
    auto clB = startClient("Player B");

    clA->executeCommand<commands::createRoomCmd>("TestRoom");
    netcode::room* roomA;
    clA->executeCommand<commands::getRoomCmd>(roomA);
    clB->executeCommand<commands::enterRoomCmd>(roomA->getId());

    int handSize = 7;
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto serverManager = std::make_shared<netGameStateManager>(events, sv);
    auto clientManagerA = std::make_shared<netGameStateManager>(events, clA);
    auto clientManagerB = std::make_shared<netGameStateManager>(events, clB);
    serverManager->bindGameEvents();

    netcode::room* roomB;
    EXPECT_TRUE(clA->executeCommand<commands::setReadyCmd>(roomA));
    std::this_thread::sleep_for(std::chrono::milliseconds(STATE_SYNC_DELAY));
    clB->executeCommand<commands::getRoomCmd>(roomB);
    EXPECT_TRUE(clB->executeCommand<commands::setReadyCmd>(roomB));

    EXPECT_TRUE(sv->isRoomReady(roomA->getId()));
    clB->executeCommand<commands::setNotReadyCmd>(roomB);
    EXPECT_FALSE(sv->isRoomReady(roomA->getId()));

    clA->executeCommand<commands::getUpdatedRoomCmd>(roomA->getId());
    clientManagerA->setupGame(roomA, handSize, "Data\\deck_setup.json", 12345);
    clientManagerA->startGame();

    EXPECT_FALSE(clB->executeCommand<commands::setNotReadyCmd>(roomB));

    closeClient(clA.get());
    closeClient(clB.get());
    closeServer(sv.get());
}
