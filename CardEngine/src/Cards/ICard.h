#pragma once
#include "../../framework.h"

namespace Cards
{
    class ENGINE_API ICard
    {
    protected:
        int number = -1;
        char color = 0;

    public:
        virtual ~ICard() = default;

        int Number() const
        {
            return number;
        }

        char Color() const
        {
            return color;
        }

        bool operator==(const ICard& other) const
        {
            return equal(other);
        }

        bool operator!=(const ICard& other) const
        {
            return !equal(other);
        }

        virtual bool equal(const ICard& other) const = 0;

        virtual bool sameColor(ICard& other) const
        {
            return color == other.color;
        }

        virtual bool sameNumber(const ICard& other) const
        {
            return number == other.number;
        }
    };
}
