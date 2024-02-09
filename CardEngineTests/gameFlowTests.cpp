#include "pch.h"

#include "Cards/baseCard.h"
#include "Cards/drawCard.h"
#include "Cards/ICard.h"
#include "Cards/reverseCard.h"
#include "Cards/skipCard.h"
#include "StateManager/gameStateManager.h"
#include "TurnSystem/IPlayer.h"

std::unique_ptr<gameStateManager> createGameManager(int players = 2, int handInitialSize = 7)
{
    std::vector<std::string> playersList = std::vector<std::string>(players);
    for (int i = 0; i < players; ++i)
    {
        playersList[i] = "Player 0" + i;
    }
    auto manager = std::make_unique<gameStateManager>(playersList, handInitialSize, "Data\\deck_setup.json", 1234);
    manager->startGame();
    return manager;
}

TEST(GameFlow, Begin)
{
    int players = 2;
    int handSize = 7;
    auto manager = createGameManager(players, handSize);
    for (int i = 0; i < players; ++i)
    {
        EXPECT_EQ(handSize, manager->getPlayer(i)->getHand().size());
    }
}

TEST(GameFlow, PlayRightCard)
{
    int players = 2;
    int handSize = 7;
    auto manager = createGameManager(players, handSize);

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

    EXPECT_TRUE(manager->tryExecutePlayerAction(currentPlayer->pickCard(index)));
    EXPECT_NE(currentPlayer, manager->getCurrentPlayer());
    EXPECT_LT(currentPlayer->getHand().size(), handSize);
}

TEST(GameFlow, PlayWrongCard)
{
    int players = 2;
    int handSize = 7;
    auto manager = createGameManager(players, handSize);

    turnSystem::IPlayer* currentPlayer = manager->getCurrentPlayer();
    cards::ICard* topCard = manager->getTopCard();
    std::list<cards::ICard*> playerCards = currentPlayer->getHand();

    int index = 0;
    for (auto card : playerCards)
    {
        if (!card->sameColor(*topCard) && !card->sameNumber(*topCard) && !card->hasAction())
        {
            break;
        }
        index++;
    }

    cards::ICard* selectedCard = currentPlayer->pickCard(0);
    EXPECT_FALSE(manager->tryExecutePlayerAction(selectedCard));
    EXPECT_EQ(currentPlayer, manager->getCurrentPlayer());
    EXPECT_LT(currentPlayer->getHand().size(), handSize);

    currentPlayer->receiveCard(selectedCard);
    EXPECT_EQ(currentPlayer->getHand().size(), handSize);
}

TEST(GameFlow, PlayReverse)
{
    auto manager = createGameManager(3);

    turnSystem::IPlayer* currentPlayer = manager->getCurrentPlayer();
    turnSystem::IPlayer* firstPlayer = currentPlayer;

    cards::ICard* topCard = manager->getTopCard();

    std::unique_ptr<cards::ICard> baseCard = std::make_unique<cards::baseCard>(topCard->Number(), 'e');
    EXPECT_TRUE(manager->tryExecutePlayerAction(baseCard.get()));

    currentPlayer = manager->getCurrentPlayer();
    topCard = manager->getTopCard();

    EXPECT_EQ(*baseCard, *topCard);
    std::unique_ptr<cards::ICard> reverseCard = std::make_unique<cards::reverseCard>(0, topCard->Color());
    EXPECT_TRUE(manager->tryExecutePlayerAction(reverseCard.get()));

    EXPECT_EQ(firstPlayer, manager->getCurrentPlayer());
    EXPECT_EQ(*reverseCard, *manager->getTopCard());
}

TEST(GameFlow, PlayDraw)
{
    int players = 2;
    int handSize = 7;
    auto manager = createGameManager(players, handSize);

    turnSystem::IPlayer* firstPlayer = manager->getCurrentPlayer();
    cards::ICard* topCard = manager->getTopCard();

    std::unique_ptr<cards::ICard> drawC = std::make_unique<cards::drawCard>(2, topCard->Color());
    EXPECT_TRUE(manager->tryExecutePlayerAction(drawC.get()));

    turnSystem::IPlayer* currentPlayer = manager->getCurrentPlayer();

    EXPECT_NE(firstPlayer, currentPlayer);
    EXPECT_EQ(handSize+2, currentPlayer->getHand().size());
    EXPECT_EQ(*drawC, *manager->getTopCard());
}

TEST(GameFlow, PlaySkip)
{
    auto manager = createGameManager(3);

    turnSystem::IPlayer* currentPlayer = manager->getCurrentPlayer();
    turnSystem::IPlayer* firstPlayer = currentPlayer;
    turnSystem::IPlayer* thirdPlayer = manager->getPlayer(2);

    cards::ICard* topCard = manager->getTopCard();

    std::unique_ptr<cards::ICard> skipCard = std::make_unique<cards::skipCard>(0, topCard->Color());
    EXPECT_TRUE(manager->tryExecutePlayerAction(skipCard.get()));

    EXPECT_EQ(thirdPlayer, manager->getCurrentPlayer());
    EXPECT_EQ(*skipCard, *manager->getTopCard());
}
