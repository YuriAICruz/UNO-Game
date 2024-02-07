#pragma once
#include "IPlayer.h"

namespace turnSystem
{
    class ITurnSystem
    {
    public:
        virtual ~ITurnSystem() = default;
        virtual IPlayer* getCurrentPlayer() const = 0;
        virtual void endTurn() = 0;
        virtual int playersCount() const = 0;
        virtual void reverse() = 0;
    };
}
