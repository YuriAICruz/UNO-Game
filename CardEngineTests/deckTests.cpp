#include "pch.h"

#include <tuple>
#include <tuple>
#include <tuple>

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

    std::unique_ptr<cards::baseCard> card = std::make_unique<cards::baseCard>(0, 1, 'b');

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

TEST(Deck, Serialize)
{
    const std::unique_ptr<decks::IDeck> deck = createDeck("Data\\test_deck_setup.json");
    deck->shuffle(1234);

    std::tuple<const char*, size_t> data = deck->getState();

    const char* ptr = std::get<0>(data);
    size_t numCards = std::get<1>(data);

    std::cout << "\nDeck Data [Begin]\n";
    for (uint8_t i = 0; i < numCards; ++i)
    {
        uint8_t id;
        std::memcpy(&id, ptr, sizeof(uint8_t));
        std::cout << std::to_string(id);
        ptr += sizeof(uint8_t);
    }
    std::cout << "\nDeck Data [End]\n";

    delete std::get<0>(data);
}

TEST(Deck, Deserialize)
{
    const std::unique_ptr<decks::IDeck> deck = createDeck("Data\\test_deck_setup.json");
    deck->shuffle();

    uint8_t list[] = {7, 6, 5, 10, 2, 9, 8, 4, 1, 3, 0};
    size_t bufferSize = sizeof(uint8_t) * std::size(list);
    char* buffer = new char[bufferSize];

    char* ptr = buffer;
    for (const auto& cardId : list)
    {
        uint8_t id = cardId;
        std::memcpy(ptr, &id, sizeof(uint8_t));
        ptr += sizeof(uint8_t);
    }

    deck->setState(buffer, bufferSize);

    for (int i = 0, n = std::size(list); i < n; ++i)
    {
        EXPECT_EQ(list[i], deck->dequeue()->Id());
    }

    EXPECT_EQ(0, deck->count());

    deck->setState(buffer, bufferSize);
    for (int i = 0, n = std::size(list); i < n; ++i)
    {
        EXPECT_EQ(list[i], deck->dequeue()->Id());
    }

    delete buffer;
}

TEST(Deck, SerializeTwoDecks)
{
    const std::unique_ptr<decks::IDeck> deck = createDeck("Data\\test_deck_setup.json");
    const std::unique_ptr<decks::IDeck> emptyDeck = std::make_unique<decks::deck>();
    deck->shuffle();
    emptyDeck->setFullDeck(deck->getFullDeck());

    int count = deck->count();
    cards::ICard* card = deck->peek();

    auto deckData = deck->getState();
    auto emptyDeckData = emptyDeck->getState();

    for (int i = 0; i < 3; ++i)
    {
        auto card = deck->dequeue();
        emptyDeck->enqueue(card);
    }
    deck->shuffle();

    deck->setState(std::get<0>(deckData), std::get<1>(deckData));
    emptyDeck->setState(std::get<0>(emptyDeckData), std::get<1>(emptyDeckData));

    EXPECT_EQ(0, emptyDeck->count());
    EXPECT_EQ(count, deck->count());
    EXPECT_EQ(*card, *deck->peek());

    emptyDeck->setState(std::get<0>(deckData), std::get<1>(deckData));
    deck->setState(std::get<0>(emptyDeckData), std::get<1>(emptyDeckData));

    EXPECT_EQ(0, deck->count());
    EXPECT_EQ(count, emptyDeck->count());
    EXPECT_EQ(*card, *emptyDeck->peek());
}
