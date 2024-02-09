#include "pch.h"

#include "Cards/baseCard.h"
#include "Cards/drawCard.h"
#include "Decks/jsonDeck.h"
#include "Cards/reverseCard.h"
#include "Cards/skipCard.h"
#include "Decks/deck.h"
#include "Decks/IDeck.h"

std::unique_ptr<decks::IDeck> createDeck(const char* path)
{
    return std::make_unique<decks::jsonDeck>(path);
}

TEST(Deck, Populate)
{
    const std::unique_ptr<decks::IDeck> deck = createDeck("Data\\deck_setup.json");
    EXPECT_EQ(104, deck->count());

    int baseCount = 0;
    int drawCount = 0;
    int skipCount = 0;
    int reverseCount = 0;

    for (auto deckCard : deck->Cards())
    {
        if (dynamic_cast<cards::baseCard*>(deckCard))
        {
            baseCount++;
        }
        else if (dynamic_cast<cards::drawCard*>(deckCard))
        {
            drawCount++;
        }
        else if (dynamic_cast<cards::reverseCard*>(deckCard))
        {
            reverseCount++;
        }
        else if (dynamic_cast<cards::skipCard*>(deckCard))
        {
            skipCount++;
        }
    }

    EXPECT_EQ(80, baseCount);
    EXPECT_EQ(8, drawCount);
    EXPECT_EQ(8, reverseCount);
    EXPECT_EQ(8, skipCount);
}

TEST(Deck, PeekAndDequeue)
{
    const std::unique_ptr<decks::IDeck> deck = createDeck("Data\\deck_setup.json");
    cards::ICard* first = deck->peek();
    unsigned int count = deck->count();

    EXPECT_EQ(first, deck->dequeue());
    EXPECT_GT(count, deck->count());
}

TEST(Deck, EnqueueAndStack)
{
    const std::unique_ptr<decks::IDeck> deck = createDeck("Data\\deck_setup.json");
    cards::ICard* first = deck->peek();

    std::unique_ptr<cards::baseCard> card = std::make_unique<cards::baseCard>(1, 'b');

    unsigned int count = deck->count();
    deck->stack(card.get());

    EXPECT_LT(count, deck->count());
    EXPECT_EQ(*card.get(), *deck->peek());
    EXPECT_NE(*first, *deck->peek());

    count = deck->count();
    cards::ICard* last = deck->peekLast();
    deck->enqueue(card.get());

    EXPECT_LT(count, deck->count());
    EXPECT_EQ(*card.get(), *deck->peekLast());
    EXPECT_NE(*last, *deck->peek());
}

TEST(Deck, Shuffle)
{
    const std::unique_ptr<decks::IDeck> deck = createDeck("Data\\deck_setup.json");
    cards::ICard* first = deck->peek();
    deck->shuffle();
    EXPECT_NE(first, deck->peek());
}
TEST(Deck, ShuffleSeed)
{
    const std::unique_ptr<decks::IDeck> deck = createDeck("Data\\deck_setup.json");
    cards::ICard* first = deck->peek();
    deck->shuffle(1234);
    EXPECT_NE(first, deck->peek());
    const std::unique_ptr<decks::IDeck> deckB = createDeck("Data\\deck_setup.json");
    deckB->shuffle(1234);
    EXPECT_NE(first, deckB->peek());
}

TEST(Deck, StackEmptyDeck)
{
    const std::unique_ptr<decks::IDeck> deck = createDeck("Data\\deck_setup.json");
    const std::unique_ptr<decks::IDeck> emptyDeck = std::make_unique<decks::deck>();

    int count = deck->count();
    for (int i = 0; i < count; ++i)
    {
        auto card = deck->dequeue();
        emptyDeck->stack(card);

        EXPECT_EQ(card, emptyDeck->peek());
    }


    EXPECT_EQ(count, emptyDeck->count());
    EXPECT_EQ(0, deck->count());
}

TEST(Deck, MoveDeck)
{
    const std::unique_ptr<decks::IDeck> deck = createDeck("Data\\deck_setup.json");
    const std::unique_ptr<decks::IDeck> emptyDeck = std::make_unique<decks::deck>();

    auto card = deck->peek();
    int count = deck->count();
    deck->moveAllCardsTo(emptyDeck.get());

    EXPECT_EQ(count, emptyDeck->count());
    EXPECT_EQ(0, deck->count());
    EXPECT_EQ(card, emptyDeck->peek());
}
