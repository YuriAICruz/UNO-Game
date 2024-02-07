#include "pch.h"
#include "localPlayer.h"

#include "Events/endTurnEventData.h"

namespace TurnSystem
{
    void localPlayer::endTurn()
    {
        events->fireEvent(0, Events::endTurnEventData{});
    }

    bool localPlayer::equal(const IPlayer& player) const
    {
        return id == player.Id();
    }
}
