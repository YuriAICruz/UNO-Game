﻿#pragma once
#include <memory>
#include "../EventBus/eventBus.h"
#include "IPlayer.h"

namespace TurnSystem
{
    class localPlayer : public IPlayer
    {
    private:
        std::shared_ptr<eventBus::eventBus> events;
    public:
        localPlayer(std::shared_ptr<eventBus::eventBus> events, size_t id) : IPlayer(id), events(events)
        {
        }

        void endTurn() override;
        bool equal(const IPlayer& player) const override;
    };
}