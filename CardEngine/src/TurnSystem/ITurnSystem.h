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
        virtual std::tuple<const char*, size_t> getState() = 0;
        virtual void setState(const char* data) = 0;
        virtual void shuffle() = 0;
        virtual void shuffle(size_t seed) = 0;
    };
}
