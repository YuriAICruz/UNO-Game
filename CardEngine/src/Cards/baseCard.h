#pragma once
#include <string>

#include "ICard.h"
#include "../../framework.h"
#include "ActionTypes/base.h"

namespace cards
{
    class ENGINE_API baseCard : public ICard
    {
    public:
        explicit baseCard(const uint8_t id, const int number, const char color)
        {
            this->id = id;
            this->number = number;
            this->color = color;
            action = std::make_unique<actions::base>();
        }

        bool equal(const ICard& other) const override
        {
            if (typeid(baseCard) != typeid(other))
            {
                return false;
            }

            return sameNumber(other) && sameColor(other) && !other.hasAction();
        }

        bool hasAction() const override
        {
            return false;
        }

        std::string typeName() const override
        {
            return "";
        }
    };
}
