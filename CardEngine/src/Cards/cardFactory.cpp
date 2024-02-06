﻿#include "pch.h"
#include "cardFactory.h"

#include <map>
#include <memory>
#include <string>

#include "baseCard.h"
#include "drawCard.h"
#include "reverseCard.h"
#include "skipCard.h"

namespace Cards
{
    template <typename Type>
    ICard* createAndAddInstance(std::vector<std::unique_ptr<ICard>>& instances, int n, char c)
    {
        instances.emplace_back(std::make_unique<Type>(n, c));
        return instances.back().get();
    }

    std::vector<std::unique_ptr<ICard>> cardFactory::instances;

    using CreateCardFunction = ICard* (*)(std::vector<std::unique_ptr<ICard>>&, int, char);
    
    constexpr size_t hash(const char* str) {
        size_t hash = 14695981039346656037ull;
        while (*str) {
            hash ^= static_cast<size_t>(*str++);
            hash *= 1099511628211ull;
        }
        return hash;
    }
    
    std::map<size_t, CreateCardFunction> classes = {
        {hash("default"), &createAndAddInstance<baseCard>},
        {hash("draw"), &createAndAddInstance<drawCard>},
        {hash("reverse"), &createAndAddInstance<reverseCard>},
        {hash("skip"), &createAndAddInstance<skipCard>}
    };

    ICard* cardFactory::Instantiate(const char* type, int number, char color)
    {
        return classes.at(hash(type))(instances, number, color);
    }
}
