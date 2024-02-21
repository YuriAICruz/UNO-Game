#pragma once
#include <xstring>

#include "IStateManager.h"
#include "../Cards/ICard.h"
#include "../Decks/IDeck.h"
#include "../TurnSystem/IPlayer.h"
#include "../TurnSystem/turnSystem.h"
#include "../../framework.h"

class ENGINE_API gameStateManager : public IStateManager
{
protected:
    std::unique_ptr<decks::IDeck> mainDeck;
    std::unique_ptr<decks::IDeck> discardDeck;
    std::unique_ptr<turnSystem::turnSystem> turner;
    size_t seed;
    uint8_t handSize;
    std::shared_ptr<eventBus::eventBus> events;
    bool running = false;
    uint8_t currentPlayerCardsDraw = 0;

public:
    gameStateManager(std::shared_ptr<eventBus::eventBus> events);
    void bindGameEvents();

    ~gameStateManager() override
    {
    }

    bool isGameRunning();
    virtual void setupGame(std::vector<std::string>& players, int handSize, std::string deckConfigFilePath,
                           size_t seed);
    virtual void setupGame(std::vector<std::string> players, std::vector<uint16_t> playersIds,
                           int handSize, std::string deckConfigFilePath, size_t seed);
    virtual void startGame();
    virtual turnSystem::IPlayer* getCurrentPlayer() const;
    virtual turnSystem::IPlayer* getNextPlayer() const;
    virtual turnSystem::IPlayer* getPlayer(int i) const;
    virtual turnSystem::IPlayer* getPlayerFromId(int id) const;
    virtual int playersCount() const;
    virtual cards::ICard* getTopCard() const;
    virtual bool makePlayerDraw(turnSystem::IPlayer* player, int count);
    virtual bool tryExecutePlayerAction(cards::ICard* card);
    virtual bool playerHasValidCardOnHand(turnSystem::IPlayer* player);
    int getStartHandSize();
    bool canYellUno();
    bool canSkipTurn() const;
    bool canDrawCard() const;
    virtual bool skipTurn();
    virtual void cheatWin();
    virtual void endGame();
    virtual void endTurn();
    virtual bool yellUno();
    std::tuple<const char*, size_t> getState() override;
    void setState(const char* data, size_t size) override;
    void print(const char* buffer, size_t size);

protected:
    void beginTurn();
    void finishAction(cards::ICard* card);
    bool isActionCardValid(cards::ICard* card, cards::ICard* topCard) const;
    bool isBaseCardValid(cards::ICard* card, cards::ICard* topCard) const;
    bool isCardValid(cards::ICard* card, cards::ICard* topCard) const;
};
