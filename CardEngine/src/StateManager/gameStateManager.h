#pragma once
#include <xstring>

#include "IStateManager.h"
#include "../Cards/ICard.h"
#include "../Decks/IDeck.h"
#include "../TurnSystem/IPlayer.h"
#include "../TurnSystem/turnSystem.h"

class gameStateManager : IStateManager
{
private:
    std::unique_ptr<decks::IDeck> mainDeck;
    std::unique_ptr<decks::IDeck> discardDeck;
    std::unique_ptr<turnSystem::turnSystem> turner;
    size_t seed;
    int handSize;

public:
    gameStateManager(std::vector<std::string>& players, int handSize, std::string deckConfigFilePath, size_t seed);
    void makePlayerDraw(turnSystem::IPlayer* player, int count);
    virtual void startGame();
    virtual turnSystem::IPlayer* getCurrentPlayer() const;
    virtual turnSystem::IPlayer* getNextPlayer() const;
    virtual turnSystem::IPlayer* getPlayer(int i) const;
    virtual cards::ICard* getTopCard() const;
    virtual bool tryExecutePlayerAction(cards::ICard* card);

private:
    void finishAction(cards::ICard* card) const;
    bool isActionCardValid(cards::ICard* card, cards::ICard* topCard) const;
    bool isCardValid(cards::ICard* card, cards::ICard* topCard) const;
};
