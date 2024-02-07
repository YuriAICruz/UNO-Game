#pragma once
#include "IStateManager.h"
#include "../Cards/ICard.h"
#include "../TurnSystem/IPlayer.h"

class gameStateManager : IStateManager
{
public:
    virtual void startGame() = 0;
    virtual TurnSystem::IPlayer* getCurrentPlayer() const = 0;
    virtual cards::ICard* getTopCard() const = 0;
    virtual bool tryExecutePlayerAction(cards::ICard* card) = 0;
};
