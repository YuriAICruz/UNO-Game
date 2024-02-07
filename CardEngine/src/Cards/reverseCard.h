#pragma once
#include "ICard.h"
#include "../../framework.h"
#include "ActionTypes/reverse.h"

namespace cards
{
    class ENGINE_API reverseCard : public ICard
    {
    public:
        explicit reverseCard(char color)
        {
            this->color = color;
            action = std::make_unique<actions::reverse>();
        }

        explicit reverseCard(int number, char color): reverseCard(color)
        {
        }

        bool equal(const ICard& other) const override
        {
            return false;
        }

        bool hasAction() const override
        {
            return true;
        }
    };
}
