#include "pch.h"
#include "Cards/drawCard.h"
#include "Cards/baseCard.h"
#include "Cards/ICard.h"
#include "Cards/reverseCard.h"
#include "Cards/skipCard.h"

TEST(Cards, numberedCardComparision)
{
    std::vector<std::unique_ptr<Cards::ICard>> cards;
    cards.emplace_back(std::make_unique<Cards::baseCard>(1, 'a'));
    cards.emplace_back(std::make_unique<Cards::baseCard>(2, 'a'));
    cards.emplace_back(std::make_unique<Cards::baseCard>(1, 'b'));
    cards.emplace_back(std::make_unique<Cards::baseCard>(2, 'b'));
    cards.emplace_back(std::make_unique<Cards::baseCard>(1, 'a'));

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
    auto nCard = Cards::baseCard(2, 'a');
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
    Cards::reverseCard rCardA = Cards::reverseCard{'a'};
    Cards::reverseCard rCardB = Cards::reverseCard{'b'};
    Cards::reverseCard rCardC = Cards::reverseCard{'a'};

    EXPECT_NE(rCardA, rCardB);
    EXPECT_TRUE(rCardA.sameNumber(rCardC));
}

TEST(Cards, skipCardComparison)
{
    Cards::skipCard rCardA = Cards::skipCard{'a'};
    Cards::skipCard rCardB = Cards::skipCard{'b'};
    Cards::skipCard rCardC = Cards::skipCard{'a'};

    EXPECT_NE(rCardA, rCardB);
    EXPECT_TRUE(rCardA.sameNumber(rCardC));
}
