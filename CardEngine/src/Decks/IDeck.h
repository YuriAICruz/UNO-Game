#pragma once
#include <list>

#include "../Cards/ICard.h"

namespace Decks
{
    class IDeck
    {
    protected:
        std::list<Cards::ICard*> cards;

    public:
        virtual unsigned int count() = 0;
        virtual Cards::ICard* peek() = 0;
        virtual Cards::ICard* peekLast() = 0;
        virtual void stack(Cards::ICard* card) =0;
        virtual void enqueue(Cards::ICard* card) =0;
        virtual Cards::ICard* dequeue() =0;
        virtual void shuffle() = 0;
        std::list<Cards::ICard*> Cards();
    };
}
