#include "pch.h"

#include "Cards/drawCard.h"
#include "Cards/numberedCard.h"
#include "Cards/ICard.h"
#include "Cards/reverseCard.h"
#include "Cards/skipCard.h"

TEST(Cards, numberedCardComparision)
{
    std::vector<Cards::ICard*> cards;
    cards.emplace_back(&Cards::numberedCard(1, 'a'));
    cards.emplace_back(&Cards::numberedCard(2, 'a'));
    cards.emplace_back(&Cards::numberedCard(1, 'b'));
    cards.emplace_back(&Cards::numberedCard(2, 'b'));
    cards.emplace_back(&Cards::numberedCard(1, 'a'));

    EXPECT_TRUE(cards[0]->sameColor(*cards[1]));
    EXPECT_TRUE(cards[2]->sameColor(*cards[3]));
    EXPECT_FALSE(cards[1]->sameColor(*cards[2]));

    EXPECT_TRUE(cards[1]->sameNumber(*cards[3]));
    EXPECT_TRUE(cards[0]->sameNumber(*cards[2]));
    EXPECT_FALSE(cards[0]->sameNumber(*cards[1]));

    EXPECT_EQ(*cards[0], *cards[4]);
}

TEST(Cards, drawCardComparison)
{
    auto nCard = Cards::numberedCard(2, 'a');
    auto dCardA = Cards::drawCard{2, 'a'};
    auto dCardB = Cards::drawCard{2, 'b'};

    EXPECT_TRUE(nCard.sameNumber(dCardA));
    EXPECT_TRUE(nCard.sameColor(dCardA));
    EXPECT_FALSE(nCard.sameColor(dCardB));

    EXPECT_NE(dCardA, dCardB);
    EXPECT_NE(nCard, dCardA);
    EXPECT_NE(dCardA, nCard);
}

TEST(Cards, reverseCardComparison)
{
    auto rCardA = Cards::reverseCard{'a'};
    auto rCardB = Cards::reverseCard{'b'};
    auto rCardC = Cards::reverseCard{'a'};

    EXPECT_NE(rCardA, rCardB);
    EXPECT_TRUE(rCardA.sameNumber(rCardC));
}

TEST(Cards, skipCardComparison)
{
    auto rCardA = Cards::skipCard{'a'};
    auto rCardB = Cards::skipCard{'b'};
    auto rCardC = Cards::skipCard{'a'};

    EXPECT_NE(rCardA, rCardB);
    EXPECT_TRUE(rCardA.sameNumber(rCardC));
}
