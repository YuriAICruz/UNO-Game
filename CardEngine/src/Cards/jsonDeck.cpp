#include "pch.h"
#include "jsonDeck.h"

#include <fstream>
#include <iostream>
#include <random>

#include "../../json.hpp"
#include "../Cards/baseCard.h"
#include "../Cards/cardFactory.h"

using json = nlohmann::json;

namespace Decks
{
    jsonDeck::jsonDeck(const char* jsonFilePath): deck()
    {
        std::ifstream file(jsonFilePath);
        json jsonData;
        file >> jsonData;
        file.close();

        for (auto data : jsonData)
        {
            std::string t = "default";
            if (data.contains("type"))
            {
                t = data["type"].get<std::string>();
            }

            int copies = data.contains("repeat") ? data["repeat"].get<int>() : 1;
            for (int i = 0; i < copies; ++i)
            {
                Cards::ICard* card = Cards::cardFactory::Instantiate(
                    t.c_str(),
                    data["number"],
                    data["color"].get<std::string>()[0]
                );
                cards.push_back(card);
            }
        }
    }
}
