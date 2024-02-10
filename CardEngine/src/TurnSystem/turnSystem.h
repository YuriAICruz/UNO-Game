#pragma once
#include <memory>
#include <string>
#include <vector>

#include "IPlayer.h"
#include "ITurnSystem.h"
#include "../EventBus/eventBus.h"
#include "Events/endTurnEventData.h"

namespace turnSystem
{
    class turnSystem : public ITurnSystem
    {
    private:
        std::vector<std::unique_ptr<IPlayer>> players;
        int currentTurn = 0;
        int playersSize;
        std::shared_ptr<eventBus::eventBus> events;
        int direction = 1;

    public:
        turnSystem(int numberOfPlayers);
        turnSystem(std::vector<std::string> numberOfPlayers);
        IPlayer* getCurrentPlayer() const override;
        IPlayer* getNextPlayer() const;
        IPlayer* getPlayer(int i) const;
        void turnEnded(Events::endTurnEventData& data);
        void endTurn() override;
        int playersCount() const override;
        void reverse() override;
    private:
        int nextTurnIndex() const;
    };
}
