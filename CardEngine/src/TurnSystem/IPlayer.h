#pragma once

namespace TurnSystem
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
        virtual void endTurn() = 0;

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
    };
}
