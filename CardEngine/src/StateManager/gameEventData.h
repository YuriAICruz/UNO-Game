#pragma once

struct gameEventData
{
    gameEventData() = default;

    explicit gameEventData(turnSystem::IPlayer* player, bool isOnline) : player(player), isOnline(isOnline)
    {
    }

    turnSystem::IPlayer* player;
    bool isOnline;
};
