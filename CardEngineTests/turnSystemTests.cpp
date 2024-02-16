#include "pch.h"

#include <tuple>
#include <tuple>
#include <tuple>
#include <tuple>

#include "Decks/jsonDeck.h"
#include "TurnSystem/turnSystem.h"

std::unique_ptr<turnSystem::ITurnSystem> createTurnSystem(int players)
{
    return std::make_unique<turnSystem::turnSystem>(players);
}

TEST(TurnSystem, Setup)
{
    auto turner = createTurnSystem(4);
    EXPECT_EQ(4, turner->playersCount());
}

TEST(TurnSystem, GetAvailablePlayer)
{
    auto turner = createTurnSystem(2);
    auto player = turner->getCurrentPlayer();
    EXPECT_TRUE(player!=nullptr);
}

TEST(TurnSystem, NextTurn)
{
    std::unique_ptr<turnSystem::ITurnSystem> turner = createTurnSystem(2);
    turnSystem::IPlayer* player = turner->getCurrentPlayer();
    EXPECT_TRUE(player!=nullptr);
    turner->endTurn();
    turnSystem::IPlayer* player2 = turner->getCurrentPlayer();
    EXPECT_NE(*player, *player2);
    player2->endTurn();
    EXPECT_EQ(*player, *turner->getCurrentPlayer());
}

TEST(TurnSystem, RunFullCircle)
{
    auto turner = createTurnSystem(10);
    int count = turner->playersCount();
    turnSystem::IPlayer* player = turner->getCurrentPlayer();
    for (int i = 0; i < count; ++i)
    {
        turner->endTurn();
    }
    EXPECT_EQ(*player, *turner->getCurrentPlayer());
}

TEST(TurnSystem, Reverse)
{
    auto turner = createTurnSystem(3);

    turnSystem::IPlayer* playerA = turner->getCurrentPlayer();
    turner->endTurn();
    EXPECT_NE(*playerA, *turner->getCurrentPlayer());

    turnSystem::IPlayer* playerB = turner->getCurrentPlayer();
    turner->reverse();
    turner->endTurn();
    EXPECT_EQ(*playerA, *turner->getCurrentPlayer());

    turner->endTurn();
    EXPECT_NE(*playerA, *turner->getCurrentPlayer());
    EXPECT_NE(*playerB, *turner->getCurrentPlayer());
}

TEST(TurnSystem, Serialize)
{
    std::unique_ptr<turnSystem::ITurnSystem> turner = createTurnSystem(12);
    turner->shuffle();
    std::tuple<const char*, size_t> data = turner->getState();

    std::cout << "\nTurnSystem Data [End]\n";
    turner->print(std::get<0>(data), std::get<1>(data));
    std::cout << "\nTurnSystem Data [End]\n";

    delete std::get<0>(data);
}

TEST(TurnSystem, Deserialize)
{
    std::unique_ptr<decks::IDeck> deck = std::make_unique<decks::jsonDeck>("Data\\test_deck_setup.json");

    auto turner = createTurnSystem(12);
    turner->shuffle();

    uint16_t currentTurn = 6;
    int8_t direction = -1;
    uint8_t playersSize = 12;
    // 0112-40905030208011070601000010
    uint16_t list[] = {4, 9, 5, 3, 2, 8, 11, 7, 6, 1, 0, 10};
    uint8_t cards[] = {0, 1, 2, 3, 4};

    size_t bufferSize =
        sizeof(uint16_t) // currentTurn
        + sizeof(int8_t) // direction
        + sizeof(uint8_t); // playersSize

    for (auto player : list)
    {
        bufferSize += sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t) * std::size(cards);
    }

    char* buffer = new char[bufferSize];

    char* ptr = buffer;
    std::memcpy(ptr, &currentTurn, sizeof(uint16_t));
    ptr += sizeof(uint16_t);
    std::memcpy(ptr, &direction, sizeof(int8_t));
    ptr += sizeof(int8_t);
    std::memcpy(ptr, &playersSize, sizeof(uint8_t));
    ptr += sizeof(uint8_t);

    uint8_t size = std::size(cards);
    for (const auto& playerId : list)
    {
        std::memcpy(ptr, &playerId, sizeof(uint16_t));
        ptr += sizeof(uint16_t);
        std::memcpy(ptr, &size, sizeof(uint8_t));
        ptr += sizeof(uint8_t);
        for (uint8_t cardId : cards)
        {
            std::memcpy(ptr, &cardId, sizeof(uint8_t));
            ptr += sizeof(uint8_t);
        }
    }

    turner->setState(buffer, deck.get());

    EXPECT_EQ(list[currentTurn], turner->getCurrentPlayer()->Id());
    turner->endTurn();
    EXPECT_EQ(list[currentTurn-1], turner->getCurrentPlayer()->Id());

    delete buffer;
}

TEST(TurnSystem, SerializeTwoSystems)
{
    std::unique_ptr<decks::IDeck> deck = std::make_unique<decks::jsonDeck>("Data\\test_deck_setup.json");

    auto turnerA = createTurnSystem(4);
    turnerA->shuffle();
    auto turnerB = createTurnSystem(6);
    turnerB->shuffle();

    int countA = turnerA->playersCount();
    int countB = turnerB->playersCount();

    uint16_t playerA = turnerA->getCurrentPlayer()->Id();
    uint16_t playerB = turnerB->getCurrentPlayer()->Id();

    auto dataA = turnerA->getState();
    auto dataB = turnerB->getState();

    turnerA->endTurn();
    turnerB->endTurn();

    EXPECT_NE(playerA, turnerA->getCurrentPlayer()->Id());
    EXPECT_NE(playerB, turnerB->getCurrentPlayer()->Id());
    EXPECT_EQ(countA, turnerA->playersCount());
    EXPECT_EQ(countB, turnerB->playersCount());

    turnerA->setState(std::get<0>(dataA), deck.get());
    turnerB->setState(std::get<0>(dataB), deck.get());

    EXPECT_EQ(playerA, turnerA->getCurrentPlayer()->Id());
    EXPECT_EQ(playerB, turnerB->getCurrentPlayer()->Id());
    EXPECT_EQ(countA, turnerA->playersCount());
    EXPECT_EQ(countB, turnerB->playersCount());

    turnerA->setState(std::get<0>(dataB), deck.get());
    turnerB->setState(std::get<0>(dataA), deck.get());

    EXPECT_EQ(playerA, turnerB->getCurrentPlayer()->Id());
    EXPECT_EQ(countA, turnerA->playersCount());
    EXPECT_NE(countB, turnerB->playersCount());

    delete std::get<0>(dataA);
    delete std::get<0>(dataB);
}
