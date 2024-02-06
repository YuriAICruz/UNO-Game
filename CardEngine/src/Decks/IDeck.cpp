#include "pch.h"
#include "IDeck.h"

namespace Decks{
    std::list<Cards::ICard*> IDeck::Cards()
    {
        return cards;
    }
}
