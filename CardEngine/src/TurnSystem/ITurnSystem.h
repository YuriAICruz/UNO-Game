#pragma once
#include "IPlayer.h"

namespace TurnSystem
{
    class ITurnSystem
    {
    public:
        virtual IPlayer* getCurrentPlayer() = 0;
        virtual void endTurn() = 0;
        virtual int playersCount() = 0;
    };
}
