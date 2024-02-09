#include "pch.h"
#include "gameStateManager.h"

#include "gameEventData.h"
#include "turnEventData.h"
#include "../coreEventIds.h"
#include "../Cards/ActionTypes/draw.h"
#include "../Cards/ActionTypes/reverse.h"
#include "../Cards/ActionTypes/skip.h"
#include "../Decks/deck.h"
#include "../Decks/jsonDeck.h"
#include "../TurnSystem/turnSystem.h"

gameStateManager::gameStateManager(std::shared_ptr<eventBus::eventBus> events): events(events)
{
    bindGameEvents();
}

bool gameStateManager::isGameStarted()
{
    return running;
}

void gameStateManager::bindGameEvents()
{
    events->bindEvent<gameEventData>(GAME_SETUP);
    events->bindEvent<gameEventData>(GAME_START);
    events->bindEvent<gameEventData>(GAME_END);
    events->bindEvent<gameEventData>(GAME_WON);
    events->bindEvent<gameEventData>(GAME_LOST);
    events->bindEvent<gameEventData>(GAME_UNO);
    
    events->bindEvent<turnEventData>(TURN_BEGIN);
    events->bindEvent<turnEventData>(TURN_END);
}

void gameStateManager::makePlayerDraw(turnSystem::IPlayer* player, int count)
{
    for (int j = 0; j < count; ++j)
    {
        player->receiveCard(mainDeck->dequeue());
    }
}

void gameStateManager::setupGame(std::vector<std::string>& players, int handSize, std::string deckConfigFilePath,
                                 size_t seed)
{
    running = false;
    this->handSize = handSize;
    mainDeck = std::make_unique<decks::jsonDeck>(deckConfigFilePath);
    discardDeck = std::make_unique<decks::deck>();

    turner = std::make_unique<turnSystem::turnSystem>(players);
    mainDeck->shuffle(seed);

    events->fireEvent(GAME_SETUP, gameEventData());
}

void gameStateManager::startGame()
{
    for (int i = 0, n = turner->playersCount(); i < n; ++i)
    {
        auto player = turner->getPlayer(i);
        makePlayerDraw(player, handSize);
    }

    auto card = mainDeck->dequeue();
    discardDeck->stack(card);

    events->fireEvent(GAME_START, gameEventData());
    running = true;
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

int gameStateManager::getStartHandSize()
{
    return handSize;
}

void gameStateManager::finishAction(cards::ICard* card) const
{
    discardDeck->stack(card);

    events->fireEvent(TURN_END, turnEventData());

    turner->endTurn();

    events->fireEvent(TURN_BEGIN, turnEventData());
}

bool gameStateManager::isActionCardValid(cards::ICard* card, cards::ICard* topCard) const
{
    return card->sameType(*topCard) || card->sameColor(*topCard);
}

bool gameStateManager::isCardValid(cards::ICard* card, cards::ICard* topCard) const
{
    return card->sameColor(*topCard) || card->sameNumber(*topCard);
}
