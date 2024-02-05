﻿#pragma once
#include "ICard.h"
#include "../../framework.h"

namespace Cards
{
    class ENGINE_API skipCard : public ICard
    {
    public:
        explicit skipCard(char color)
        {
            this->color = color;
        }

        bool equal(const ICard& other) const override
        {
            return false;
        } 
    };
}
