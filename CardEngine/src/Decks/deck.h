#pragma once
#include "IDeck.h"

namespace Decks
{
    class deck : public IDeck
    {
    public:
        deck();
        unsigned int count() override;
        Cards::ICard* peek() override;
        void shuffle() override;
        Cards::ICard* peekLast() override;
        void stack(Cards::ICard* card) override;
        void enqueue(Cards::ICard* card) override;
        Cards::ICard* dequeue() override;
    };
}
