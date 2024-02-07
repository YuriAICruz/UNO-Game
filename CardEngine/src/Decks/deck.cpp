#include "pch.h"
#include "deck.h"

#include <random>

#include "../../json.hpp"
#include "../Cards/baseCard.h"

namespace decks
{
    deck::deck()
    {
    }

    unsigned int deck::count()
    {
        return cards.size();
    }

    cards::ICard* deck::peek()
    {
        return cards.front();
    }

    void deck::shuffle()
    {
        std::random_device rng;
        shuffle(rng());
    }

    void deck::shuffle(size_t seed)
    {
        std::vector<cards::ICard*> v(cards.begin(), cards.end());
        std::mt19937 generator(seed);
        std::shuffle(v.begin(), v.end(), generator);
        cards.assign(v.begin(), v.end());
    }

    cards::ICard* deck::peekLast()
    {
        return cards.back();
    }

    void deck::stack(cards::ICard* card)
    {
        return cards.push_front(card);
    }

    void deck::enqueue(cards::ICard* card)
    {
        return cards.push_back(card);
    }

    cards::ICard* deck::dequeue()
    {
        auto card = cards.front();
        cards.pop_front();
        return card;
    }
}
