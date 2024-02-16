#pragma once
#include "IDeck.h"
#include "../../framework.h"

namespace decks
{
    class ENGINE_API deck : public IDeck
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
        void organizeDeck(std::vector<uint8_t> ids, std::list<cards::ICard*> allCards) override;
    };
}
