#include "pch.h"
#include "IDeck.h"
#include <tuple>
#include <vector>

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

    std::tuple<const char*, size_t> IDeck::getState() const
    {
        size_t bufferSize = sizeof(uint8_t) * cards.size();
        char* buffer = new char[bufferSize];

        char* ptr = buffer;
        for (const auto& card : cards)
        {
            uint8_t id = card->Id();
            std::memcpy(ptr, &id, sizeof(uint8_t));
            ptr += sizeof(uint8_t);
        }
        return std::make_tuple(buffer, bufferSize);
    }

    void IDeck::setState(const char* data, size_t size)
    {
        const char* ptr = data;
        size_t numCards = size / sizeof(uint8_t);

        std::vector<uint8_t> cards;
        cards.resize(numCards);
        for (int i = 0; i < numCards; ++i)
        {
            uint8_t id;
            std::memcpy(&id, ptr, sizeof(uint8_t));
            cards[i] = id;
            ptr += sizeof(uint8_t);
        }

        organizeDeck(cards, allCards);
    }

    std::list<cards::ICard*> IDeck::getFullDeck()
    {
        return allCards;
    }

    void IDeck::setFullDeck(const std::list<cards::ICard*>& fullDeck)
    {
        allCards = fullDeck;
    }
}
