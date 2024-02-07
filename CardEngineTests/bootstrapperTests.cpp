#include "pch.h"

#include "Bootstrapper/bootstrapper.h"
#include "Decks/IDeck.h"
#include "Decks/jsonDeck.h"
#include "TurnSystem/IPlayer.h"
#include "TurnSystem/localPlayer.h"

TEST(Bootstrapper, Bind)
{
    std::unique_ptr<bootstrapper> strapper = std::make_unique<bootstrapper>();
    strapper->bind<decks::IDeck, std::string>()->to<decks::jsonDeck>();
}

TEST(Bootstrapper, Construct)
{
    std::unique_ptr<bootstrapper> strapper = std::make_unique<bootstrapper>();
    strapper->bind<decks::IDeck, std::string>()->to<decks::jsonDeck>();

    std::string str = std::string("Data\\deck_setup.json");
    std::shared_ptr<decks::IDeck> deck = strapper->create<decks::IDeck, const std::string&>(str);
    EXPECT_EQ(104, deck->count());
}
