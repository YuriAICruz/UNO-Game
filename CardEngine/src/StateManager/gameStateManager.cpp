#include "pch.h"
#include "gameStateManager.h"

#include <tuple>
#include <iostream>

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
}

bool gameStateManager::isGameRunning()
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
    events->bindEvent<gameEventData>(GAME_NO_UNO_PENALTY);

    events->bindEvent<turnEventData>(TURN_BEGIN);
    events->bindEvent<turnEventData>(TURN_END);
}

void gameStateManager::beginTurn()
{
    currentPlayerCardsDraw = 0;

    auto player = getCurrentPlayer();
    if (player->getHand().size() < 2 && !player->isInUnoMode())
    {
        events->fireEvent(GAME_NO_UNO_PENALTY, gameEventData());
        makePlayerDraw(player, 2);
    }
    else if (player->isInUnoMode())
    {
        player->resetUnoMode();
    }

    events->fireEvent(TURN_BEGIN, turnEventData());
}

void gameStateManager::makePlayerDraw(turnSystem::IPlayer* player, int count)
{
    for (int j = 0; j < count; ++j)
    {
        currentPlayerCardsDraw++;
        player->receiveCard(mainDeck->dequeue());
    }
}

void gameStateManager::setupGame(std::vector<std::string>& players, int handSize,
                                 std::string deckConfigFilePath, size_t seed)
{
    running = false;
    this->seed = seed;
    this->handSize = handSize;
    mainDeck = std::make_unique<decks::jsonDeck>(deckConfigFilePath);
    discardDeck = std::make_unique<decks::deck>();

    turner = std::make_unique<turnSystem::turnSystem>(players);
    mainDeck->shuffle(this->seed);

    events->fireEvent(GAME_SETUP, gameEventData());
}

void gameStateManager::setupGame(std::vector<std::string>& players, std::vector<size_t>& playersIds, int handSize,
                                 std::string deckConfigFilePath, size_t seed)
{
    running = false;
    this->seed = seed;
    this->handSize = handSize;
    mainDeck = std::make_unique<decks::jsonDeck>(deckConfigFilePath);
    discardDeck = std::make_unique<decks::deck>();
    discardDeck->setFullDeck(mainDeck->getFullDeck());

    turner = std::make_unique<turnSystem::turnSystem>(players, playersIds);
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
    beginTurn();
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

    if (isBaseCardValid(card, topCard))
    {
        finishAction(card);
        return true;
    }
}

bool gameStateManager::playerHasValidCardOnHand(turnSystem::IPlayer* player)
{
    auto hand = player->getHand();
    auto top = getTopCard();
    for (auto card : hand)
    {
        if (isCardValid(card, top))
        {
            return true;
        }
    }

    return false;
}

int gameStateManager::getStartHandSize()
{
    return handSize;
}

bool gameStateManager::canYellUno()
{
    return getCurrentPlayer()->getHand().size() == 2 && !getCurrentPlayer()->isInUnoMode();
}

void gameStateManager::yellUno()
{
    events->fireEvent(GAME_UNO, gameEventData());
    getCurrentPlayer()->setUnoMode();
}

std::tuple<const char*, size_t> gameStateManager::getState()
{
    size_t size = sizeof(size_t) + sizeof(uint8_t) + sizeof(uint8_t);
    std::tuple<const char*, size_t> md = mainDeck->getState();
    size += sizeof(size_t) + std::get<1>(md);
    std::tuple<const char*, size_t> dd = discardDeck->getState();
    size += sizeof(size_t) + std::get<1>(dd);
    std::tuple<const char*, size_t> t = turner->getState();
    size += sizeof(size_t) + std::get<1>(t);

    char* buffer = new char[size];
    char* ptr = buffer;

    std::memcpy(ptr, &seed, sizeof(size_t));
    ptr += sizeof(size_t);
    std::memcpy(ptr, &currentPlayerCardsDraw, sizeof(uint8_t));
    ptr += sizeof(uint8_t);
    std::memcpy(ptr, &handSize, sizeof(uint8_t));
    ptr += sizeof(uint8_t);

    std::memcpy(ptr, &std::get<1>(md), sizeof(size_t));
    ptr += sizeof(size_t);
    std::memcpy(ptr, std::get<0>(md), std::get<1>(md) * sizeof(uint8_t));
    ptr += std::get<1>(md) * sizeof(uint8_t);

    std::memcpy(ptr, &std::get<1>(dd), sizeof(size_t));
    ptr += sizeof(size_t);
    std::memcpy(ptr, std::get<0>(dd), std::get<1>(dd) * sizeof(uint8_t));
    ptr += std::get<1>(dd) * sizeof(uint8_t);

    std::memcpy(ptr, &std::get<1>(t), sizeof(size_t));
    ptr += sizeof(size_t);
    std::memcpy(ptr, std::get<0>(t), std::get<1>(t));
    ptr += std::get<1>(t);

    delete std::get<0>(md);
    delete std::get<0>(dd);
    delete std::get<0>(t);

    return std::make_tuple(buffer, size);
}

void gameStateManager::setState(const char* data, size_t size)
{
    const char* ptr = data;

    std::memcpy(&seed, ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    std::memcpy(&currentPlayerCardsDraw, ptr, sizeof(uint8_t));
    ptr += sizeof(uint8_t);
    std::memcpy(&handSize, ptr, sizeof(uint8_t));
    ptr += sizeof(uint8_t);

    size_t mdSize;
    std::memcpy(&mdSize, ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    mainDeck->setState(ptr, mdSize);
    ptr += mdSize;

    size_t ddSize;
    std::memcpy(&ddSize, ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    discardDeck->setState(ptr, ddSize);
    ptr += ddSize;

    size_t tSize;
    std::memcpy(&tSize, ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    turner->setState(ptr, mainDeck.get());
    ptr += tSize;
}

void gameStateManager::print(const char* buffer, size_t size)
{
    const char* ptr = buffer;

    size_t s;
    std::memcpy(&s, ptr, sizeof(size_t));
    std::cout << s;
    ptr += sizeof(size_t);
    uint8_t cpcd;
    std::memcpy(&cpcd, ptr, sizeof(uint8_t));
    std::cout << std::to_string(cpcd);
    ptr += sizeof(uint8_t);
    uint8_t handS;
    std::memcpy(&handS, ptr, sizeof(uint8_t));
    std::cout << std::to_string(handS);
    ptr += sizeof(uint8_t);

    std::cout << "-md-";
    size_t mdSize;
    std::memcpy(&mdSize, ptr, sizeof(size_t));
    std::cout << std::to_string(mdSize);
    std::cout << "-";
    ptr += sizeof(size_t);
    mainDeck->print(ptr, mdSize);
    ptr += mdSize * sizeof(uint8_t);

    std::cout << "-dd-";
    size_t ddSize;
    std::memcpy(&ddSize, ptr, sizeof(size_t));
    std::cout << std::to_string(ddSize);
    std::cout << "-";
    ptr += sizeof(size_t);
    discardDeck->print(ptr, ddSize);
    ptr += ddSize * sizeof(uint8_t);

    std::cout << "-t-";
    size_t tSize;
    std::memcpy(&tSize, ptr, sizeof(size_t));
    std::cout << std::to_string(tSize);
    std::cout << "-";
    ptr += sizeof(size_t);
    turner->print(ptr, tSize);
    ptr += tSize;
}

bool gameStateManager::canSkipTurn() const
{
    return currentPlayerCardsDraw > 0;
}

bool gameStateManager::canDrawCard() const
{
    return currentPlayerCardsDraw == 0;
}

void gameStateManager::skipTurn()
{
    endTurn();
}

void gameStateManager::cheatWin()
{
    endGame();
}

void gameStateManager::endGame()
{
    running = false;
    events->fireEvent(GAME_END, gameEventData(getCurrentPlayer()));
}

void gameStateManager::finishAction(cards::ICard* card)
{
    discardDeck->stack(card);

    endTurn();
}

void gameStateManager::endTurn()
{
    events->fireEvent(TURN_END, turnEventData());

    if (getCurrentPlayer()->getHand().size() <= 0)
    {
        endGame();
    }

    turner->endTurn();

    beginTurn();
}

bool gameStateManager::isActionCardValid(cards::ICard* card, cards::ICard* topCard) const
{
    return card->sameType(*topCard) || card->sameColor(*topCard);
}

bool gameStateManager::isBaseCardValid(cards::ICard* card, cards::ICard* topCard) const
{
    return card->sameColor(*topCard) || card->sameNumber(*topCard);
}

bool gameStateManager::isCardValid(cards::ICard* card, cards::ICard* topCard) const
{
    if (card->hasAction())
    {
        if (isActionCardValid(card, topCard) && card->actionType()->isEqual(typeid(cards::actions::draw)))
        {
            return true;
        }
        if (isActionCardValid(card, topCard) && card->actionType()->isEqual(typeid(cards::actions::skip)))
        {
            return true;
        }
        if (isActionCardValid(card, topCard) && card->actionType()->isEqual(typeid(cards::actions::reverse)))
        {
            return true;
        }
        return false;
    }

    if (isBaseCardValid(card, topCard))
    {
        return true;
    }
}
