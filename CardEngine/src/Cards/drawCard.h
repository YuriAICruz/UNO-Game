#pragma once

#include <vcruntime_typeinfo.h>

#include "ICard.h"
#include "../../framework.h"

namespace Cards
{
    class ENGINE_API drawCard : public ICard
    {
    public:
        explicit drawCard(int drawCount, char color)
        {
            this->number = drawCount;
            this->color = color;
        }
        
        bool equal(const ICard& other) const override
        {
            if (color != other.Color())
            {
                return false;
            }

            if(typeid(drawCard) == typeid(other))
            {
                return number == other.Number();
            }

            return false;
        }

        bool sameNumber(ICard& other) const override
        {
            return false;
        }
    };
}
