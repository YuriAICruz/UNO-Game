#include "pch.h"

#include "TurnSystem/turnSystem.h"

std::unique_ptr<TurnSystem::ITurnSystem> createTurnSystem(int players)
{
    return std::make_unique<TurnSystem::turnSystem>(players);
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
    std::unique_ptr<TurnSystem::ITurnSystem> turner = createTurnSystem(2);
    TurnSystem::IPlayer* player = turner->getCurrentPlayer();
    EXPECT_TRUE(player!=nullptr);
    turner->endTurn();
    TurnSystem::IPlayer* player2 = turner->getCurrentPlayer();
    EXPECT_NE(*player, *player2);
    player2->endTurn();
    EXPECT_EQ(*player, *turner->getCurrentPlayer());
}

TEST(TurnSystem, RunFullCircle)
{
    auto turner = createTurnSystem(10);
    int count = turner->playersCount();
    TurnSystem::IPlayer* player = turner->getCurrentPlayer();
    for (int i = 0; i < count; ++i)
    {
        turner->endTurn();
    }
    EXPECT_EQ(*player, *turner->getCurrentPlayer());
}
