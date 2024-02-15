#pragma once
#include <list>
#include <memory>
#include <string>

#include "../../framework.h"
#include "../EventBus/eventBus.h"
#include "IPlayer.h"

namespace turnSystem
{
    class ENGINE_API localPlayer : public IPlayer
    {
    private:
        std::list<cards::ICard*> hand;
        std::shared_ptr<eventBus::eventBus> events;

    public:
        localPlayer(std::string name, std::shared_ptr<eventBus::eventBus> events, uint16_t id) : IPlayer(id, name),
            events(events)
        {
        }

        void endTurn() override;
        bool equal(const IPlayer& player) const override;
        std::list<cards::ICard*> getHand() const override;
        cards::ICard* pickCard(int index) override;
        void receiveCard(cards::ICard* card) override;
    };
}
