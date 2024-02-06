﻿#pragma once
#include <memory>
#include <vector>

#include "IPlayer.h"
#include "ITurnSystem.h"
#include "../EventBus/eventBus.h"
#include "Events/endTurnEventData.h"

namespace TurnSystem
{
    class turnSystem : public ITurnSystem
    {
    private:
        std::vector<std::unique_ptr<IPlayer>> players;
        int currentTurn = 0;
        int playersSize;
        std::shared_ptr<eventBus::eventBus> events;

    public:
        turnSystem(int numberOfPlayers);
        IPlayer* getCurrentPlayer() const override;
        void turnEnded(Events::endTurnEventData& data);
        void endTurn() override;
        int playersCount() const override;
    };
}