#pragma once
#include <map>
#include <memory>
#include <vector>

#include "ICard.h"

namespace cards
{
    class cardFactory
    {
    private:
        static std::vector<std::unique_ptr<ICard>> instances;

    public:
        static ICard* Instantiate(const char* type, int number, char color);
    };
}
