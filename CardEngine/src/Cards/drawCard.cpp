#include "pch.h"
#include "drawCard.h"

namespace cards
{
    bool drawCard::equal(const ICard& other) const
    {
        return sameNumber(other) && sameColor(other);
    }

    bool drawCard::sameNumber(const ICard& other) const
    {
        return sameType(other) && this->number == other.Number();
    }
}
