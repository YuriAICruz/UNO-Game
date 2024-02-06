#include "pch.h"
#include "deck.h"

#include <fstream>
#include <iostream>
#include <random>

#include "../../json.hpp"
#include "../Cards/baseCard.h"
#include "../Cards/cardFactory.h"

using json = nlohmann::json;

namespace Decks
{
    deck::deck(const char* jsonFilePath)
    {
        std::ifstream file(jsonFilePath);
        json jsonData;
        file >> jsonData;
        file.close();

        for (auto data : jsonData)
        {
            auto t = data.contains("type") ? data["type"].get<std::string>().c_str() : "";
            int copies = data.contains("repeat") ? data["repeat"].get<int>() : 1;
            for (int i = 0; i < copies; ++i)
            {
                Cards::ICard* card = Cards::cardFactory::Instantiate(
                    t,
                    data["number"],
                    data["color"].get<std::string>()[0]
                );
                cards.push_back(card);
            }
        }
    }

    unsigned int deck::count()
    {
        return cards.size();
    }

    Cards::ICard* deck::peek()
    {
        return cards.front();
    }

    void deck::shuffle()
    {
        std::vector<Cards::ICard*> v(cards.begin(), cards.end());
        std::random_device rng;
        std::mt19937 generator(rng());
        std::shuffle(v.begin(), v.end(), generator);
        cards.assign(v.begin(), v.end());
    }

    Cards::ICard* deck::peekLast()
    {
        return cards.back();
    }

    void deck::stack(Cards::ICard* card)
    {
        return cards.push_front(card);
    }

    void deck::enqueue(Cards::ICard* card)
    {
        return cards.push_back(card);
    }

    Cards::ICard* deck::dequeue()
    {
        auto card = cards.front();
        cards.pop_front();
        return card;
    }
}
