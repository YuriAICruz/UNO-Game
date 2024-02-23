#pragma once
#include "TurnSystem/IPlayer.h"

namespace screens
{
    struct transitionData
    {
        transitionData() = default;
        explicit transitionData(turnSystem::IPlayer* player): player(player){}
        turnSystem::IPlayer* player;
    };
}
