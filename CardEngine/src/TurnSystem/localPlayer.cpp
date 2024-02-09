#include "pch.h"
#include "localPlayer.h"

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
}
