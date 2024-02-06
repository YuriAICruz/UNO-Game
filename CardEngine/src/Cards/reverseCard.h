#pragma once
#include "ICard.h"
#include "../../framework.h"

namespace Cards
{
    class ENGINE_API reverseCard : public ICard
    {
    public:
        explicit reverseCard(char color)
        {
            this->color = color;
        }

        explicit reverseCard(int number, char color): reverseCard(color)
        {
        }

        bool equal(const ICard& other) const override
        {
            return false;
        }
    };
}
