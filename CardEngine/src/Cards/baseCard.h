#pragma once
#include <vcruntime_typeinfo.h>
#include "ICard.h"
#include "../../framework.h"

namespace cards
{
    class ENGINE_API baseCard : public ICard
    {
    public:
        explicit baseCard(const int number, const char color)
        {
            this->number = number;
            this->color = color;
        }

        bool equal(const ICard& other) const override
        {
            if (typeid(baseCard) != typeid(other))
            {
                return false;
            }

            return number == other.Number() && color == other.Color();
        }

        bool hasAction() const override
        {
            return false;
        }
    };
}
