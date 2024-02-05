#include "pch.h"
#include "deck.h"

namespace Decks{
    deck::deck(const char* jsonFilePath)
    {
    }

    unsigned int deck::count()
    {
        return 0;
    }

    Cards::ICard* deck::peek()
    {
        return nullptr;
    }

    void deck::shuffle()
    {
    }

    Cards::ICard* deck::peekLast()
    {
        return nullptr;
    }

    void deck::stack(Cards::ICard* card)
    {
    }

    void deck::enqueue(Cards::ICard* card)
    {
    }

    Cards::ICard* deck::dequeue()
    {
        return nullptr;
    }
}
