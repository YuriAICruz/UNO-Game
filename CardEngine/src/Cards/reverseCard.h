#pragma once
#include <string>
#include "ICard.h"
#include "../../framework.h"
#include "ActionTypes/reverse.h"

namespace cards
{
    class ENGINE_API reverseCard : public ICard
    {
    public:
        explicit reverseCard(const uint8_t id, char color)
        {
            this->id = id;
            this->color = color;
            action = std::make_unique<actions::reverse>();
        }

        explicit reverseCard(const uint8_t id, int number, char color): reverseCard(id,color)
        {
        }

        bool equal(const ICard& other) const override
        {
            return sameType(other) && sameColor(other);
        }

        bool hasAction() const override
        {
            return true;
        }

        std::string typeName() const override
        {
            return "rev";
        }
    };
}
