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

    turnSystem::turnSystem(std::vector<std::string> playersNames, std::vector<uint16_t> playersId)
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

    IPlayer* turnSystem::getPlayerFromId(uint16_t id) const
    {
        for (auto player : players)
        {
            if (player->Id() == id)
            {
                return player.get();
            }
        }

        return nullptr;
    }

    std::vector<uint16_t> turnSystem::getPlayersIds()
    {
        std::vector<uint16_t> list;
        list.resize(playersCount());

        for (int i = 0, n = playersCount(); i < n; ++i)
        {
            list[i] = players[i]->Id();
        }

        return list;
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

    void turnSystem::organizePlayers(std::vector<playerData> playersIds, decks::IDeck* deck)
    {
        std::vector<std::shared_ptr<IPlayer>> newPlayers;

        std::unordered_map<uint8_t, std::vector<std::shared_ptr<IPlayer>>::iterator> idToIteratorMap;
        for (auto it = players.begin(); it != players.end(); ++it)
        {
            idToIteratorMap[(*it)->Id()] = it;
        }

        for (auto playerData : playersIds)
        {
            auto it = idToIteratorMap.find(playerData.id);
            if (it != idToIteratorMap.end())
            {
                std::vector<std::shared_ptr<IPlayer>>::iterator c = it->second;
                std::shared_ptr<IPlayer> player = *c;
                player->organizeHand(playerData.hand, deck);
                newPlayers.push_back(player);
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
            + sizeof(uint8_t); // playersSize

        for (auto player : players)
        {
            bufferSize += sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t) * player->getHand().size();
        }

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
            uint8_t handSize = player->getHand().size();
            std::memcpy(ptr, &handSize, sizeof(uint8_t));
            ptr += sizeof(uint8_t);
            for (const auto& card : player->getHand())
            {
                uint8_t cardId = card->Id();
                std::memcpy(ptr, &cardId, sizeof(uint8_t));
                ptr += sizeof(uint8_t);
            }
        }

        return std::make_tuple(buffer, bufferSize);
    }

    void turnSystem::setState(const char* data, decks::IDeck* deck)
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
        std::vector<playerData> playersIds;
        playersIds.resize(size);
        for (int i = 0; i < size; ++i)
        {
            uint16_t id;
            std::memcpy(&id, ptr, sizeof(uint16_t));
            playersIds[i].id = id;
            ptr += sizeof(uint16_t);
            uint8_t handSize;
            std::memcpy(&handSize, ptr, sizeof(uint8_t));
            playersIds[i].hand.resize(handSize);
            ptr += sizeof(uint8_t);
            for (int j = 0; j < handSize; ++j)
            {
                uint8_t cardId;
                std::memcpy(&cardId, ptr, sizeof(uint8_t));
                playersIds[i].hand[j] = cardId;
                ptr += sizeof(uint8_t);
            }
        }

        organizePlayers(playersIds, deck);
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
            std::cout << std::to_string(id) << ".";
            ptr += sizeof(uint16_t);
            uint8_t handSize;
            std::memcpy(&handSize, ptr, sizeof(uint8_t));
            std::cout << std::to_string(handSize);
            ptr += sizeof(uint8_t);
            for (int j = 0; j < handSize; ++j)
            {
                uint8_t cardId;
                std::memcpy(&cardId, ptr, sizeof(uint8_t));
                std::cout << std::to_string(cardId);
                ptr += sizeof(uint8_t);
            }
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
