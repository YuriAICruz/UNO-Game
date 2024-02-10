#pragma once

struct gameEventData
{
    gameEventData() = default;

    explicit gameEventData(turnSystem::IPlayer* player) : player(player)
    {
    }

    turnSystem::IPlayer* player;
};
