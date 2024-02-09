#include "pch.h"

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
