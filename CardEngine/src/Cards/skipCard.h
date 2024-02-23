#pragma once
#include <string>

#include "ICard.h"
#include "../../framework.h"
#include "ActionTypes/skip.h"

namespace cards
{
    class ENGINE_API skipCard : public ICard
    {
    public:
        explicit skipCard(const uint8_t id, char color)
        {
            this->id = id;
            this->color = color;
            action = std::make_unique<actions::skip>();
        }

        explicit skipCard(const uint8_t id, int number, char color): skipCard(id, color)
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
            return "skip";
        }
    };
}
