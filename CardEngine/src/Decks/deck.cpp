#include "pch.h"
#include "deck.h"

#include <random>

#include "../../json.hpp"
#include "../Cards/baseCard.h"

namespace Decks
{
    deck::deck()
    {
    }

    unsigned int deck::count()
    {
        return cards.size();
    }

    Cards::ICard* deck::peek()
    {
        return cards.front();
    }

    void deck::shuffle()
    {
        std::vector<Cards::ICard*> v(cards.begin(), cards.end());
        std::random_device rng;
        std::mt19937 generator(rng());
        std::shuffle(v.begin(), v.end(), generator);
        cards.assign(v.begin(), v.end());
    }

    Cards::ICard* deck::peekLast()
    {
        return cards.back();
    }

    void deck::stack(Cards::ICard* card)
    {
        return cards.push_front(card);
    }

    void deck::enqueue(Cards::ICard* card)
    {
        return cards.push_back(card);
    }

    Cards::ICard* deck::dequeue()
    {
        auto card = cards.front();
        cards.pop_front();
        return card;
    }
}
