#pragma once
#include "../Decks/deck.h"

namespace Decks
{
    class jsonDeck : public Decks::deck
    {
    public:
        explicit jsonDeck(const char* jsonFilePath);
    };
}
