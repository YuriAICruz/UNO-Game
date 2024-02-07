#pragma once
#include "ICard.h"
#include "../../framework.h"
#include "ActionTypes/draw.h"

namespace cards
{
    class ENGINE_API drawCard : public ICard
    {
    public:
        explicit drawCard(int drawCount, char color)
        {
            this->number = drawCount;
            this->color = color;
            action = std::make_unique<actions::draw>();
        }

        bool equal(const ICard& other) const override;

        bool sameNumber(const ICard& other) const override;

        bool hasAction() const override
        {
            return true;
        }
    };
}
