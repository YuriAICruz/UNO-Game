#include "pch.h"
#include "cardFactory.h"

#include <memory>

#include "baseCard.h"
#include "drawCard.h"
#include "reverseCard.h"
#include "skipCard.h"

namespace Cards
{
    std::vector<std::unique_ptr<ICard>> cardFactory::instances;

    ICard* cardFactory::Instantiate(const char* value, int number, char color)
    {
        if (strcmp(drawCard::TypeId(), value) == 0)
        {
            auto up = std::make_unique<drawCard>(number, color);
            instances.push_back(std::move(up));
            return instances.back().get();
        }
        if (strcmp(reverseCard::TypeId(), value) == 0)
        {
            auto up = std::make_unique<reverseCard>(color);
            instances.push_back(std::move(up));
            return instances.back().get();
        }
        if (strcmp(skipCard::TypeId(), value) == 0)
        {
            auto up = std::make_unique<skipCard>(color);
            instances.push_back(std::move(up));
            return instances.back().get();
        }

        auto up = std::make_unique<baseCard>(number, color);
        instances.push_back(std::move(up));
        return instances.back().get();
    }
}
