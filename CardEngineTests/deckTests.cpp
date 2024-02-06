#include "pch.h"

#include "Cards/baseCard.h"
#include "Decks/deck.h"
#include "Decks/IDeck.h"

std::unique_ptr<Decks::IDeck> createDeck(const char* path)
{
    return std::make_unique<Decks::deck>(path);
}

TEST(Deck, Populate)
{
    const std::unique_ptr<Decks::IDeck> deck = createDeck("Data\\deck_setup.json");
    EXPECT_EQ(104, deck->count());
}

TEST(Deck, PeekAndDequeue)
{
    const std::unique_ptr<Decks::IDeck> deck = createDeck("Data\\deck_setup.json");
    Cards::ICard* first = deck->peek();
    unsigned int count = deck->count();

    EXPECT_EQ(first, deck->dequeue());
    EXPECT_GT(count, deck->count());
}

TEST(Deck, EnqueueAndStack)
{
    const std::unique_ptr<Decks::IDeck> deck = createDeck("Data\\deck_setup.json");
    Cards::ICard* first = deck->peek();

    std::unique_ptr<Cards::baseCard> card = std::make_unique<Cards::baseCard>(1, 'b');

    unsigned int count = deck->count();
    deck->stack(card.get());
    
    EXPECT_LT(count, deck->count());
    EXPECT_EQ(*card.get(), *deck->peek());
    EXPECT_NE(*first, *deck->peek());
    
    count = deck->count();
    Cards::ICard* last = deck->peekLast();
    deck->enqueue(card.get());
    
    EXPECT_LT(count, deck->count());
    EXPECT_EQ(*card.get(), *deck->peekLast());
    EXPECT_NE(*last, *deck->peek());
}

TEST(Deck, Shuffle)
{
    const std::unique_ptr<Decks::IDeck> deck = createDeck("Data\\deck_setup.json");
    Cards::ICard* first = deck->peek();
    deck->shuffle();
    EXPECT_NE(first, deck->peek());
}
