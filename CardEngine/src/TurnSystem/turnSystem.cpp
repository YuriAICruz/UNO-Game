#include "pch.h"
#include "turnSystem.h"

#include <memory>

#include "localPlayer.h"
#include "../EventBus/eventBus.h"
#include "Events/endTurnEventData.h"

namespace turnSystem
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

    IPlayer* turnSystem::getNextPlayer() const
    {
        return players[nextTurnIndex()].get();
    }

    IPlayer* turnSystem::getPlayer(int i) const
    {
        return players[i].get();
    }

    void turnSystem::turnEnded(Events::endTurnEventData& data)
    {
        endTurn();
    }

    void turnSystem::endTurn()
    {
        currentTurn = nextTurnIndex();
    }

    int turnSystem::playersCount() const
    {
        return playersSize;
    }

    void turnSystem::reverse()
    {
        direction *= -1;
    }

    int turnSystem::nextTurnIndex() const
    {
        int index = (currentTurn + direction) % playersCount();
        if (index < 0)
        {
            index = playersCount() + index;
        }
        return index;
    }
}
