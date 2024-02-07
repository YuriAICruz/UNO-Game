#include "pch.h"

#include "Cards/ICard.h"
#include "Cards/ActionTypes/draw.h"
#include "Cards/ActionTypes/reverse.h"
#include "StateManager/gameStateManager.h"
#include "TurnSystem/IPlayer.h"

TEST(GameFlow, Begin)
{
    // auto manager = std::make_unique<gameStateManager>();
    // manager->startGame();
    // TurnSystem::IPlayer* firstPlayer = manager->getCurrentPlayer();
    // TurnSystem::IPlayer* currentPlayer = manager->getCurrentPlayer();
    // cards::ICard* topCard = manager->getTopCard();
    // std::list<cards::ICard*> playerCards = currentPlayer->getHand();
    //
    // int index = 0;
    // for (auto card : playerCards)
    // {
    //     if (card->sameColor(*topCard) || card->sameNumber(*topCard))
    //     {
    //         break;
    //     }
    //     index++;
    // }
    //
    // EXPECT_TRUE(manager->tryExecutePlayerAction(currentPlayer->selectCard(index)));
    // EXPECT_NE(currentPlayer, manager->getCurrentPlayer());
    //
    // currentPlayer = manager->getCurrentPlayer();
    // topCard = manager->getTopCard();
    // playerCards = currentPlayer->getHand();
    //
    // index = 0;
    // for (auto card : playerCards)
    // {
    //     if (!card->sameColor(*topCard) && !card->sameNumber(*topCard))
    //     {
    //         break;
    //     }
    //     index++;
    // }
    //
    // cards::ICard* selectedCard = currentPlayer->pickCard(0);
    // EXPECT_FALSE(manager->tryExecutePlayerAction(selectedCard));
    // currentPlayer->receiveCard(selectedCard);
    //
    // index = 0;
    // for (auto card : playerCards)
    // {
    //     if (card->hasAction() && card->actionType()->isEqual(typeid(cards::actions::reverse)))
    //     {
    //         break;
    //     }
    //     index++;
    // }
    //
    // // play a reverse card
    // EXPECT_TRUE(manager->tryExecutePlayerAction(currentPlayer->pickCard(1)));
    //
    // EXPECT_EQ(firstPlayer, currentPlayer);
}
