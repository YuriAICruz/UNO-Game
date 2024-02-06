#pragma once
#include <memory>
#include <vector>

#include "ICard.h"

namespace Cards
{
    class cardFactory
    {
    private:
        static std::vector<std::unique_ptr<ICard>> instances;
    public:
        static ICard* Instantiate(const char* value, int number, char color);
    };
}
