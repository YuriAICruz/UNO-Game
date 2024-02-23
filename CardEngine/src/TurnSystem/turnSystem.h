#pragma once
#include <memory>
#include <string>
#include <vector>

#include "IPlayer.h"
#include "ITurnSystem.h"
#include "../EventBus/eventBus.h"
#include "../../framework.h"
#include "Events/endTurnEventData.h"

namespace turnSystem
{
    class ENGINE_API turnSystem : public ITurnSystem
    {
    private:
        struct playerData
        {
            uint16_t id;
            std::vector<uint8_t> hand;
        };
        std::vector<std::shared_ptr<IPlayer>> players;
        uint16_t currentTurn = 0;
        uint8_t playersSize;
        std::shared_ptr<eventBus::eventBus> events;
        int8_t direction = 1;

    public:
        turnSystem(int numberOfPlayers);
        turnSystem(std::vector<std::string> numberOfPlayers);
        turnSystem(std::vector<std::string> players, std::vector<uint16_t> playersId);
        IPlayer* getCurrentPlayer() const override;
        IPlayer* getNextPlayer() const;
        IPlayer* getPlayer(int i) const;
        IPlayer* getPlayerFromId(int id) const;
        std::vector<uint16_t> getPlayersIds();
        void shuffle() override;
        void shuffle(size_t seed) override;
        void turnEnded(Events::endTurnEventData& data);
        void endTurn() override;
        int playersCount() const override;
        void reverse() override;
        void organizePlayers(std::vector<playerData> playersIds, decks::IDeck* deck);
        std::tuple<const char*, size_t> getState() override;
        void setState(const char* data, decks::IDeck* deck) override;
        void print(const char* buffer, size_t size) override;

    private:
        int nextTurnIndex() const;

    public:
    };
}
