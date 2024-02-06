#pragma once
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

        bool equal(const ICard& other) const override;

        bool sameNumber(ICard& other) const override;
    };
}
