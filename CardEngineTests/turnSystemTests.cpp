#include "pch.h"

#include <tuple>
#include <tuple>
#include <tuple>

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
    auto turner = createTurnSystem(12);
    turner->shuffle();
    auto data = turner->getState();

    std::cout << "\nTurnSystem Data [End]\n";
    turner->print(std::get<0>(data), std::get<1>(data));
    std::cout << "\nTurnSystem Data [End]\n";

    delete std::get<0>(data);
}

TEST(TurnSystem, Deserialize)
{
    auto turner = createTurnSystem(12);
    turner->shuffle();

    uint16_t currentTurn = 6;
    int8_t direction = -1;
    uint8_t playersSize = 12;
    uint16_t list[] = {3, 2, 1, 0, 6, 9, 11, 7, 5, 8, 4, 10};

    size_t bufferSize =
        sizeof(uint16_t) // currentTurn
        + sizeof(int8_t) // direction
        + sizeof(uint8_t) // playersSize
        + sizeof(uint16_t) * playersSize;

    char* buffer = new char[bufferSize];

    char* ptr = buffer;
    std::memcpy(ptr, &currentTurn, sizeof(uint16_t));
    ptr += sizeof(uint16_t);
    std::memcpy(ptr, &direction, sizeof(int8_t));
    ptr += sizeof(int8_t);
    std::memcpy(ptr, &playersSize, sizeof(uint8_t));
    ptr += sizeof(uint8_t);

    for (const auto& playerId : list)
    {
        uint16_t id = playerId;
        std::memcpy(ptr, &id, sizeof(uint16_t));
        ptr += sizeof(uint16_t);
    }

    turner->setState(buffer);

    EXPECT_EQ(list[currentTurn], turner->getCurrentPlayer()->Id());
    turner->endTurn();
    EXPECT_EQ(list[currentTurn-1], turner->getCurrentPlayer()->Id());

    delete buffer;
}

TEST(TurnSystem, SerializeTwoSystems)
{
    auto turnerA = createTurnSystem(4);
    turnerA->shuffle();
    auto turnerB = createTurnSystem(6);
    turnerB->shuffle();

    int countA = turnerA->playersCount();
    int countB = turnerB->playersCount();

    auto playerA = turnerA->getCurrentPlayer();
    auto playerB = turnerB->getCurrentPlayer();

    auto dataA = turnerA->getState();
    auto dataB = turnerB->getState();

    turnerA->endTurn();
    turnerB->endTurn();

    EXPECT_NE(playerA, turnerA->getCurrentPlayer());
    EXPECT_NE(playerB, turnerB->getCurrentPlayer());
    EXPECT_EQ(countA, turnerA->playersCount());
    EXPECT_EQ(countB, turnerB->playersCount());

    turnerA->setState(std::get<0>(dataA));
    turnerB->setState(std::get<0>(dataB));

    EXPECT_EQ(playerA, turnerA->getCurrentPlayer());
    EXPECT_EQ(playerB, turnerB->getCurrentPlayer());
    EXPECT_EQ(countA, turnerA->playersCount());
    EXPECT_EQ(countB, turnerB->playersCount());

    turnerA->setState(std::get<0>(dataB));
    turnerB->setState(std::get<0>(dataA));

    EXPECT_EQ(*playerB, *turnerA->getCurrentPlayer());
    EXPECT_EQ(*playerA, *turnerB->getCurrentPlayer());
    EXPECT_EQ(countA, turnerA->playersCount());
    EXPECT_NE(countB, turnerB->playersCount());
}
