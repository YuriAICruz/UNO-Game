#include "pch.h"
#include "IDeck.h"

namespace decks
{
    std::list<cards::ICard*> IDeck::Cards()
    {
        return cards;
    }

    void IDeck::moveAllCardsTo(IDeck* deck)
    {
        int c = count();
        for (int i = 0; i < c; ++i)
        {
            auto card = dequeue();
            deck->enqueue(card);
        }
    }
}
