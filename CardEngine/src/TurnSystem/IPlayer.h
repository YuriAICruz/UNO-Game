﻿#pragma once
#include <list>
#include <string>
#include <vector>

#include "../../framework.h"
#include "../Cards/ICard.h"
#include "../Decks/IDeck.h"

namespace turnSystem
{
    class ENGINE_API IPlayer
    {
    protected:
        uint16_t id;
        std::string name;
        bool inUnoMode = false;

    public:
        IPlayer(uint16_t id, std::string name) : id(id), name(name)
        {
        }

        virtual ~IPlayer() = default;

        virtual bool equal(const IPlayer& player) const = 0;

        bool operator==(const IPlayer& player) const
        {
            return equal(player);
        }

        bool operator!=(const IPlayer& player) const
        {
            return !equal(player);
        }

        uint16_t Id() const
        {
            return id;
        }

        std::string getName()
        {
            return name;
        }

        virtual void endTurn() = 0;
        virtual std::list<cards::ICard*> getHand() const = 0;
        virtual cards::ICard* pickCard(int index) = 0;
        virtual void receiveCard(cards::ICard* card) = 0;
        virtual void organizeHand(std::vector<uint8_t> hand, decks::IDeck* deck) = 0;

        virtual void setUnoMode()
        {
            inUnoMode = true;
        }

        virtual void resetUnoMode()
        {
            inUnoMode = false;
        }

        virtual bool isInUnoMode()
        {
            return inUnoMode;
        }
    };
}
