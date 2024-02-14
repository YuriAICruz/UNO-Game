#pragma once
#include "IPlayer.h"

namespace turnSystem
{
    class ENGINE_API ITurnSystem
    {
    public:
        virtual ~ITurnSystem() = default;
        virtual IPlayer* getCurrentPlayer() const = 0;
        virtual void endTurn() = 0;
        virtual int playersCount() const = 0;
        virtual void reverse() = 0;
    };
}
