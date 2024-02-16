#pragma once
#include <list>
#include <memory>
#include <vector>

#include "../Cards/ICard.h"
#include "../../framework.h"

namespace decks
{
    class ENGINE_API IDeck
    {
    protected:
        std::list<cards::ICard*> cards;
        std::list<cards::ICard*> allCards;

    public:
        virtual unsigned int count() = 0;
        virtual cards::ICard* peek() = 0;
        virtual cards::ICard* peekLast() = 0;
        virtual void stack(cards::ICard* card) =0;
        virtual void enqueue(cards::ICard* card) =0;
        virtual cards::ICard* dequeue() =0;
        virtual void shuffle() = 0;
        virtual void shuffle(size_t seed) = 0;
        virtual void organizeDeck(std::vector<uint8_t> ids, std::list<cards::ICard*> allCards) = 0;
        std::list<cards::ICard*> Cards();
        void moveAllCardsTo(IDeck* deck);
        std::tuple<const char*, size_t> getState() const;
        void setState(const char* data, size_t size);
        std::list<cards::ICard*> getFullDeck();
        void setFullDeck(const std::list<cards::ICard*>& list);
        static void print(const char* buffer, size_t size);
    };
}
