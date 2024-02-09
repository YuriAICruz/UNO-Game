#pragma once
#include <string>

#include "../Decks/deck.h"

namespace decks
{
    class jsonDeck : public deck
    {
    public:
        explicit jsonDeck(const std::string& jsonFilePath) : jsonDeck(jsonFilePath.c_str())
        {
        }
        explicit jsonDeck(const char* jsonFilePath);
    };
}
