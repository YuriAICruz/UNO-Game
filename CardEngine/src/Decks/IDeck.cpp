#include "pch.h"
#include "IDeck.h"

namespace Decks
{
    std::list<Cards::ICard*> IDeck::Cards()
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
