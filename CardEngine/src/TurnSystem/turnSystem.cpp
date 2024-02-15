#include "pch.h"
#include "turnSystem.h"

#include <memory>
#include <random>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "localPlayer.h"
#include "../EventBus/eventBus.h"
#include "Events/endTurnEventData.h"

namespace turnSystem
{
    turnSystem::turnSystem(int numberOfPlayers)
    {
        events = std::make_shared<eventBus::eventBus>();
        events->bindEvent<Events::endTurnEventData&>(0);
        events->subscribe<Events::endTurnEventData&>(0, std::bind(&turnSystem::turnEnded, this, std::placeholders::_1));

        playersSize = numberOfPlayers;

        for (int i = 0; i < playersSize; ++i)
        {
            std::stringstream ss;
            ss << "Player 0" << i;
            std::string str = ss.str();
            players.emplace_back(std::make_shared<localPlayer>(str, events, i));
        }
    }

    turnSystem::turnSystem(std::vector<std::string> numberOfPlayers)
    {
        events = std::make_shared<eventBus::eventBus>();
        events->bindEvent<Events::endTurnEventData&>(0);
        events->subscribe<Events::endTurnEventData&>(0, std::bind(&turnSystem::turnEnded, this, std::placeholders::_1));

        playersSize = numberOfPlayers.size();

        for (int i = 0; i < playersSize; ++i)
        {
            players.emplace_back(std::make_shared<localPlayer>(numberOfPlayers[i], events, i));
        }
    }

    turnSystem::turnSystem(std::vector<std::string> playersNames, std::vector<size_t> playersId)
    {
        events = std::make_shared<eventBus::eventBus>();
        events->bindEvent<Events::endTurnEventData&>(0);
        events->subscribe<Events::endTurnEventData&>(0, std::bind(&turnSystem::turnEnded, this, std::placeholders::_1));

        playersSize = playersId.size();

        for (int i = 0; i < playersSize; ++i)
        {
            players.emplace_back(std::make_shared<localPlayer>(playersNames[i], events, playersId[i]));
        }
    }

    IPlayer* turnSystem::getCurrentPlayer() const
    {
        return players[currentTurn].get();
    }

    IPlayer* turnSystem::getNextPlayer() const
    {
        return players[nextTurnIndex()].get();
    }

    IPlayer* turnSystem::getPlayer(int i) const
    {
        return players[i].get();
    }

    void turnSystem::shuffle()
    {
        std::random_device rng;
        shuffle(rng());
    }

    void turnSystem::shuffle(size_t seed)
    {
        std::vector<std::shared_ptr<IPlayer>> v(players.begin(), players.end());
        std::mt19937 generator(seed);
        std::shuffle(v.begin(), v.end(), generator);
        players.assign(v.begin(), v.end());
    }

    void turnSystem::turnEnded(Events::endTurnEventData& data)
    {
        endTurn();
    }

    void turnSystem::endTurn()
    {
        currentTurn = nextTurnIndex();
    }

    int turnSystem::playersCount() const
    {
        return playersSize;
    }

    void turnSystem::reverse()
    {
        direction *= -1;
    }

    void turnSystem::organizePlayers(std::vector<uint16_t> playersIds)
    {
        std::vector<std::shared_ptr<IPlayer>> newPlayers;

        std::unordered_map<uint8_t, std::vector<std::shared_ptr<IPlayer>>::iterator> idToIteratorMap;
        for (auto it = players.begin(); it != players.end(); ++it)
        {
            idToIteratorMap[(*it)->Id()] = it;
        }

        for (auto id : playersIds)
        {
            auto it = idToIteratorMap.find(id);
            if (it != idToIteratorMap.end())
            {
                std::vector<std::shared_ptr<IPlayer>>::iterator c = it->second;
                newPlayers.push_back(*c);
            }
        }

        players.clear();
        players.swap(newPlayers);
        playersSize = players.size();
    }

    std::tuple<const char*, size_t> turnSystem::getState()
    {
        size_t bufferSize =
            sizeof(uint16_t) // currentTurn
            + sizeof(int8_t) // direction
            + sizeof(uint8_t) // playersSize
            + sizeof(uint16_t) * playersSize;

        char* buffer = new char[bufferSize];

        char* ptr = buffer;
        std::memcpy(ptr, &currentTurn, sizeof(uint16_t));
        ptr += sizeof(uint16_t);
        std::memcpy(ptr, &direction, sizeof(int8_t));
        ptr += sizeof(int8_t);
        std::memcpy(ptr, &playersSize, sizeof(uint8_t));
        ptr += sizeof(uint8_t);
        for (const auto& player : players)
        {
            uint16_t id = player->Id();
            std::memcpy(ptr, &id, sizeof(uint16_t));
            ptr += sizeof(uint16_t);
        }

        return std::make_tuple(buffer, bufferSize);
    }

    void turnSystem::setState(const char* data)
    {
        const char* ptr = data;
        std::memcpy(&currentTurn, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);
        std::memcpy(&direction, ptr, sizeof(int8_t));
        ptr += sizeof(int8_t);
        uint8_t pSize;
        std::memcpy(&pSize, ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);

        int size = pSize;
        std::vector<uint16_t> playersIds;
        playersIds.resize(size);
        for (int i = 0; i < size; ++i)
        {
            uint16_t id;
            std::memcpy(&id, ptr, sizeof(uint16_t));
            playersIds[i] = id;
            ptr += sizeof(uint16_t);
        }

        organizePlayers(playersIds);
    }

    void turnSystem::print(const char* buffer, size_t size)
    {
        const char* ptr = buffer;
        uint16_t currentTurn;
        std::memcpy(&currentTurn, ptr, sizeof(uint16_t));
        ptr += sizeof(uint16_t);
        uint8_t direction;
        std::memcpy(&direction, ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);
        uint8_t playersSize;
        std::memcpy(&playersSize, ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);
        std::cout << std::to_string(currentTurn) << std::to_string(direction) << std::to_string(playersSize) << "-";

        for (int i = 0; i < playersSize; ++i)
        {
            uint16_t id;
            std::memcpy(&id, ptr, sizeof(uint16_t));
            std::cout << id;
            ptr += sizeof(uint16_t);
        }
    }

    int turnSystem::nextTurnIndex() const
    {
        int index = (currentTurn + direction) % playersCount();
        if (index < 0)
        {
            index = playersCount() + index;
        }
        return index;
    }
}
