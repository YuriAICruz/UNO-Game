#pragma once
#include "ICard.h"
#include "../../framework.h"

namespace Cards
{
    class ENGINE_API reverseCard : public ICard
    {
    private:
        static const char* typeId;

    public:
        explicit reverseCard(char color)
        {
            this->color = color;
        }

        bool equal(const ICard& other) const override
        {
            return false;
        }

        static const char* TypeId();
    };
}
