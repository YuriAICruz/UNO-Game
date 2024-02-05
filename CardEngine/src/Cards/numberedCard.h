﻿#pragma once
#include <vcruntime_typeinfo.h>
#include "ICard.h"
#include "../../framework.h"

namespace Cards
{
    class ENGINE_API numberedCard : public ICard
    {
    public:
        explicit numberedCard(const int number, const char color)
        {
            this->number = number;
            this->color = color;
        }

        bool equal(const ICard& other) const override
        {
            if (typeid(numberedCard) != typeid(other))
            {
                auto a = typeid(this).name();
                auto b = typeid(other).name();
                return false;
            }

            return number == other.Number() && color == other.Color();
        }
    };
}
