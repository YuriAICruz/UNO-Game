#include "pch.h"
#include "Cards/drawCard.h"
#include "Cards/baseCard.h"
#include "Cards/ICard.h"
#include "Cards/reverseCard.h"
#include "Cards/skipCard.h"
#include "Cards/ActionTypes/draw.h"
#include "Cards/ActionTypes/reverse.h"
#include "Cards/ActionTypes/skip.h"

TEST(Cards, numberedCardComparision)
{
    std::vector<std::unique_ptr<cards::ICard>> cards;
    cards.emplace_back(std::make_unique<cards::baseCard>(1, 'a'));
    cards.emplace_back(std::make_unique<cards::baseCard>(2, 'a'));
    cards.emplace_back(std::make_unique<cards::baseCard>(1, 'b'));
    cards.emplace_back(std::make_unique<cards::baseCard>(2, 'b'));
    cards.emplace_back(std::make_unique<cards::baseCard>(1, 'a'));

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
    auto nCard = std::make_unique<cards::baseCard>(2, 'a');
    auto dCardA = std::make_unique<cards::drawCard>(2, 'a');
    auto dCardB = std::make_unique<cards::drawCard>(2, 'b');

    EXPECT_TRUE(nCard->sameNumber(*dCardA));
    EXPECT_TRUE(nCard->sameColor(*dCardA));
    EXPECT_FALSE(nCard->sameColor(*dCardB));

    EXPECT_NE(*dCardA, *dCardB);
    EXPECT_NE(*nCard, *dCardA);
    EXPECT_NE(*dCardA, *nCard);
}

TEST(Cards, reverseCardComparison)
{
    auto rCardA = std::make_unique<cards::reverseCard>('a');
    auto rCardB = std::make_unique<cards::reverseCard>('b');
    auto rCardC =std::make_unique< cards::reverseCard>('a');

    EXPECT_NE(*rCardA, *rCardB);
    EXPECT_TRUE(rCardA->sameNumber(*rCardC));
}

TEST(Cards, skipCardComparison)
{
    auto rCardA = std::make_unique<cards::skipCard>('a');
    auto rCardB = std::make_unique<cards::skipCard>('b');
    auto rCardC = std::make_unique<cards::skipCard>('a');

    EXPECT_NE(*rCardA, *rCardB);
    EXPECT_TRUE(rCardA->sameNumber(*rCardC));
}

TEST(Cards, actions)
{
    auto sCard = std::make_unique<cards::skipCard>('a');
    auto rCard = std::make_unique<cards::reverseCard>('a');
    auto dCard = std::make_unique<cards::drawCard>(2, 'a');
    auto bCard = std::make_unique<cards::baseCard>(1, 'a');

    EXPECT_TRUE(sCard->hasAction());
    EXPECT_TRUE(rCard->hasAction());
    EXPECT_TRUE(dCard->hasAction());
    EXPECT_FALSE(bCard->hasAction());

    EXPECT_FALSE(sCard->sameType(*rCard));
    EXPECT_FALSE(sCard->sameType(*dCard));
    EXPECT_FALSE(sCard->sameType(*bCard));
    EXPECT_FALSE(rCard->sameType(*dCard));
    EXPECT_FALSE(rCard->sameType(*bCard));
    EXPECT_FALSE(dCard->sameType(*bCard));

    EXPECT_TRUE(sCard->actionType()->isEqual(typeid(cards::actions::skip)));
    EXPECT_TRUE(rCard->actionType()->isEqual(typeid(cards::actions::reverse)));
    EXPECT_TRUE(dCard->actionType()->isEqual(typeid(cards::actions::draw)));
}
