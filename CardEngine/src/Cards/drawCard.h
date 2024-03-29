﻿#pragma once
#include <string>

#include "ICard.h"
#include "../../framework.h"
#include "ActionTypes/draw.h"

namespace cards
{
    class ENGINE_API drawCard : public ICard
    {
    public:
        explicit drawCard(const uint8_t id, int drawCount, char color)
        {
            this->id = id;
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

        std::string typeName() const override
        {
            return "draw";
        }
    };
}
