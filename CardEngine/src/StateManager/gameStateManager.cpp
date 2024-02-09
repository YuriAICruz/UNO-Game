#include "pch.h"
#include "gameStateManager.h"

#include "../Cards/ActionTypes/draw.h"
#include "../Cards/ActionTypes/reverse.h"
#include "../Cards/ActionTypes/skip.h"
#include "../Decks/deck.h"
#include "../Decks/jsonDeck.h"
#include "../TurnSystem/turnSystem.h"

gameStateManager::gameStateManager(std::vector<std::string> players, int handSize, std::string deckConfigFilePath,
                                   size_t seed) : seed(seed), handSize(handSize)
{
    mainDeck = std::make_unique<decks::jsonDeck>(deckConfigFilePath);
    discardDeck = std::make_unique<decks::deck>();

    turner = std::make_unique<turnSystem::turnSystem>(players);
}

void gameStateManager::makePlayerDraw(turnSystem::IPlayer* player, int count)
{
    for (int j = 0; j < count; ++j)
    {
        player->receiveCard(mainDeck->dequeue());
    }
}

void gameStateManager::startGame()
{
    mainDeck->shuffle(seed);
    for (int i = 0, n = turner->playersCount(); i < n; ++i)
    {
        auto player = turner->getPlayer(i);
        makePlayerDraw(player, handSize);
    }

    auto card = mainDeck->dequeue();
    discardDeck->stack(card);
}

turnSystem::IPlayer* gameStateManager::getCurrentPlayer() const
{
    return turner->getCurrentPlayer();
}

turnSystem::IPlayer* gameStateManager::getNextPlayer() const
{
    return turner->getNextPlayer();
}

turnSystem::IPlayer* gameStateManager::getPlayer(int i) const
{
    return turner->getPlayer(i);
}

cards::ICard* gameStateManager::getTopCard() const
{
    return discardDeck->peek();
}

bool gameStateManager::tryExecutePlayerAction(cards::ICard* card)
{
    auto topCard = getTopCard();

    if (card->hasAction())
    {
        if (isActionCardValid(card, topCard) && card->actionType()->isEqual(typeid(cards::actions::draw)))
        {
            makePlayerDraw(getNextPlayer(), card->Number());
            finishAction(card);
            return true;
        }
        if (isActionCardValid(card, topCard) && card->actionType()->isEqual(typeid(cards::actions::skip)))
        {
            turner->endTurn();
            finishAction(card);
            return true;
        }
        if (isActionCardValid(card, topCard) && card->actionType()->isEqual(typeid(cards::actions::reverse)))
        {
            turner->reverse();
            finishAction(card);
            return true;
        }
        return false;
    }

    if (isCardValid(card, topCard))
    {
        finishAction(card);
        return true;
    }
}

void gameStateManager::finishAction(cards::ICard* card) const
{
    discardDeck->stack(card);
    turner->endTurn();
}

bool gameStateManager::isActionCardValid(cards::ICard* card, cards::ICard* topCard) const
{
    return card->sameType(*topCard) || card->sameColor(*topCard);
}

bool gameStateManager::isCardValid(cards::ICard* card, cards::ICard* topCard) const
{
    return card->sameColor(*topCard) || card->sameNumber(*topCard);
}
