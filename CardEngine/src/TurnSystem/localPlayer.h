#pragma once
#include <list>
#include <memory>
#include <string>

#include "../EventBus/eventBus.h"
#include "IPlayer.h"

namespace turnSystem
{
    class localPlayer : public IPlayer
    {
    private:
        std::list<cards::ICard*> hand;
        std::shared_ptr<eventBus::eventBus> events;
        std::string name;

    public:
        localPlayer(std::string name, std::shared_ptr<eventBus::eventBus> events, size_t id) : name(name), IPlayer(id),
            events(events)
        {
        }

        std::string getName()
        {
            return name;
        }

        void endTurn() override;
        bool equal(const IPlayer& player) const override;
        std::list<cards::ICard*> getHand() const override;
        cards::ICard* pickCard(int index) override;
        void receiveCard(cards::ICard* card) override;
    };
}
