#include "pch.h"
#include "turnSystem.h"

#include <memory>

#include "localPlayer.h"
#include "../EventBus/eventBus.h"
#include "Events/endTurnEventData.h"

namespace TurnSystem
{
    turnSystem::turnSystem(int numberOfPlayers)
    {
        events = std::make_shared<eventBus::eventBus>();
        events->bindEvent<Events::endTurnEventData&>(0);
        events->subscribe<Events::endTurnEventData&>(0, std::bind(&turnSystem::turnEnded, this, std::placeholders::_1));

        playersSize = numberOfPlayers;

        for (int i = 0; i < playersSize; ++i)
        {
            players.emplace_back(std::make_unique<localPlayer>(events, i));
        }
    }

    IPlayer* turnSystem::getCurrentPlayer() const
    {
        return players[currentTurn].get();
    }

    void turnSystem::turnEnded(Events::endTurnEventData& data)
    {
        endTurn();
    }

    void turnSystem::endTurn()
    {
        currentTurn = (currentTurn + 1) % playersCount();
    }

    int turnSystem::playersCount() const
    {
        return playersSize;
    }
}
