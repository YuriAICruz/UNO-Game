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
        explicit skipCard(char color)
        {
            this->color = color;
            action = std::make_unique<actions::skip>();
        }

        explicit skipCard(int number, char color): skipCard(color)
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
