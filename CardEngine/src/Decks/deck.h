#pragma once
#include "IDeck.h"

namespace decks
{
    class deck : public IDeck
    {
    public:
        deck();
        unsigned int count() override;
        cards::ICard* peek() override;
        void shuffle() override;
        void shuffle(size_t seed) override;
        cards::ICard* peekLast() override;
        void stack(cards::ICard* card) override;
        void enqueue(cards::ICard* card) override;
        cards::ICard* dequeue() override;
    };
}
