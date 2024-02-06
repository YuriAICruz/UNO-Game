#include "pch.h"
#include "drawCard.h"
#include <vcruntime_typeinfo.h>

namespace Cards
{
    bool drawCard::equal(const ICard& other) const
    {
        if (color != other.Color())
        {
            return false;
        }

        if (typeid(drawCard) == typeid(other))
        {
            return number == other.Number();
        }

        return false;
    }

    bool drawCard::sameNumber(ICard& other) const
    {
        return false;
    }
}
