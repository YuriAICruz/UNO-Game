#pragma once
#include <list>
#include <memory>

#include "../Cards/ICard.h"

namespace decks
{
    class IDeck
    {
    protected:
        std::list<cards::ICard*> cards;

    public:
        virtual unsigned int count() = 0;
        virtual cards::ICard* peek() = 0;
        virtual cards::ICard* peekLast() = 0;
        virtual void stack(cards::ICard* card) =0;
        virtual void enqueue(cards::ICard* card) =0;
        virtual cards::ICard* dequeue() =0;
        virtual void shuffle() = 0;
        virtual void shuffle(size_t seed) = 0;
        std::list<cards::ICard*> Cards();
        void moveAllCardsTo(decks::IDeck* deck);
    };
}
