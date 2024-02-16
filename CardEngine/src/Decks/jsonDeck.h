#pragma once
#include <string>

#include "../Decks/deck.h"
#include "../../framework.h"

namespace decks
{
    class ENGINE_API jsonDeck : public deck
    {
    public:
        explicit jsonDeck(const std::string& jsonFilePath) : jsonDeck(jsonFilePath.c_str())
        {
        }
        explicit jsonDeck(const char* jsonFilePath);
    };
}
