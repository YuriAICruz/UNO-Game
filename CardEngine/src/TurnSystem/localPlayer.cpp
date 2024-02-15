#include "pch.h"
#include "localPlayer.h"

#include <unordered_map>

#include "Events/endTurnEventData.h"

namespace turnSystem
{
    void localPlayer::endTurn()
    {
        events->fireEvent(0, Events::endTurnEventData{});
    }

    bool localPlayer::equal(const IPlayer& player) const
    {
        return id == player.Id();
    }

    std::list<cards::ICard*> localPlayer::getHand() const
    {
        return hand;
    }

    cards::ICard* localPlayer::pickCard(int index)
    {
        std::list<cards::ICard*>::iterator it = hand.begin();
        std::advance(it, index);
        auto card = *it;
        hand.erase(it);
        return card;
    }

    void localPlayer::receiveCard(cards::ICard* card)
    {
        hand.push_back(card);
    }

    void localPlayer::organizeHand(std::vector<uint8_t> handIds, decks::IDeck* deck)
    {
        std::list<cards::ICard*> newHand;

        std::list<cards::ICard*> cards = deck->getFullDeck();
        std::unordered_map<uint8_t, std::list<cards::ICard*>::iterator> idToIteratorMap;
        for (auto it = cards.begin(); it != cards.end(); ++it)
        {
            idToIteratorMap[(*it)->Id()] = it;
        }

        for (auto cardId : handIds)
        {
            auto it = idToIteratorMap.find(cardId);
            if (it != idToIteratorMap.end())
            {
                std::list<cards::ICard*>::iterator c = it->second;
                newHand.push_back(*c);
            }
        }

        hand.clear();
        hand.swap(newHand);
    }
}
