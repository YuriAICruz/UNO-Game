#include "pch.h"

#include <tuple>
#include <tuple>
#include <tuple>

#include "Cards/baseCard.h"
#include "Cards/drawCard.h"
#include "Cards/ICard.h"
#include "Cards/reverseCard.h"
#include "Cards/skipCard.h"
#include "StateManager/gameStateManager.h"
#include "TurnSystem/IPlayer.h"

std::unique_ptr<gameStateManager> createGameManager(int players = 2, int handInitialSize = 7, int seed = 1234)
{
    std::vector<std::string> playersList = std::vector<std::string>(players);
    for (int i = 0; i < players; ++i)
    {
        std::stringstream ss;
        ss << "Player 0" << i;
        playersList[i] = ss.str();
    }
    std::shared_ptr<eventBus::eventBus> events = std::make_unique<eventBus::eventBus>();
    auto manager = std::make_unique<gameStateManager>(events);
    manager->bindGameEvents();
    manager->setupGame(playersList, handInitialSize, "Data\\deck_setup.json", seed);
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

    std::unique_ptr<cards::ICard> baseCard = std::make_unique<cards::baseCard>(0, topCard->Number(), 'e');
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

    std::unique_ptr<cards::ICard> drawC = std::make_unique<cards::drawCard>(1, 2, topCard->Color());
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

TEST(GameFlow, SerializeStates)
{
    auto manager = createGameManager(3, 7, 1234);

    std::tuple<const char*, size_t> startData = manager->getState();

    std::cout << "\nGame State [Begin]\n";
    manager->print(std::get<0>(startData), std::get<1>(startData));
    std::cout << "\nGame State [End]\n";

    cards::ICard* topCard = manager->getTopCard();

    std::unique_ptr<cards::ICard> skipCard = std::make_unique<cards::skipCard>(0, topCard->Color());
    EXPECT_TRUE(manager->tryExecutePlayerAction(skipCard.get()));

    EXPECT_EQ(*skipCard, *manager->getTopCard());

    auto endData = manager->getState();
    std::cout << "\nGame State [Begin]\n";
    manager->print(std::get<0>(endData), std::get<1>(endData));
    std::cout << "\nGame State [End]\n";

    delete std::get<0>(startData);
    delete std::get<0>(endData);
}

TEST(GameFlow, StateWarp)
{
    int handSize = 7;
    auto managerA = createGameManager(3, handSize, 1234);
    auto managerB = createGameManager(3, handSize, 4321);

    std::tuple<const char*, size_t> startDataA = managerA->getState();
    std::tuple<const char*, size_t> startDataB = managerA->getState();

    auto playerA = managerA->getCurrentPlayer();
    auto cardsA = playerA->getHand();

    for (int i = 0; i < handSize; ++i)
    {
        playerA->pickCard(0);
    }

    auto playerB = managerB->getCurrentPlayer();
    auto cardsB = playerA->getHand();

    auto endDataA = managerA->getState();
    auto endDataB = managerB->getState();

    EXPECT_EQ(0, managerA->getCurrentPlayer()->getHand().size());
    EXPECT_EQ(handSize, managerB->getCurrentPlayer()->getHand().size());

    managerA->setState(std::get<0>(startDataA), std::get<1>(startDataA));
    EXPECT_EQ(handSize, managerA->getCurrentPlayer()->getHand().size());

    managerA->setState(std::get<0>(startDataA), std::get<1>(startDataB));

    auto hand = managerA->getCurrentPlayer()->getHand();

    auto it1 = cardsB.begin();
    auto it2 = hand.begin();

    while (it1 != cardsB.end() && it2 != hand.end())
    {
        cards::ICard* cardA = *it1;
        cards::ICard* cardB = *it1;
        EXPECT_EQ(*cardA, *cardB);
        ++it1;
        ++it2;
    }

    delete std::get<0>(startDataA);
    delete std::get<0>(endDataA);

    delete std::get<0>(startDataB);
    delete std::get<0>(endDataB);
}
