#pragma once
#include "../Decks/deck.h"

namespace decks
{
    class jsonDeck : public deck
    {
    public:
        explicit jsonDeck(const char* jsonFilePath);
    };
}
