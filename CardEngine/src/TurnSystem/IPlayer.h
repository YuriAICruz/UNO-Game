#pragma once
#include <list>
#include "../Cards/ICard.h"

namespace turnSystem
{
    class IPlayer
    {
    protected:
        size_t id;

    public:
        IPlayer(size_t id) : id(id)
        {
        }

        virtual ~IPlayer() = default;

        virtual bool equal(const IPlayer& player) const = 0;

        bool operator==(const IPlayer& player) const
        {
            return equal(player);
        }

        bool operator!=(const IPlayer& player) const
        {
            return !equal(player);
        }

        size_t Id() const
        {
            return id;
        }

        virtual void endTurn() = 0;
        virtual std::list<cards::ICard*> getHand() const = 0;
        virtual cards::ICard* pickCard(int index) = 0;
        virtual void receiveCard(cards::ICard* card) = 0;
    };
}
