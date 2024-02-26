#pragma once
#include "netCommands.h"
#include "commands/client/clientCommand.h"
#include "../../netGameStateManager.h"

namespace commands
{
    class NET_ENGINE_API drawCardsCmd : public clientCommand
    {
    private:
        netGameStateManager* gameManager;
        int cardsCount;
        uint16_t playerId;

    public:
        drawCardsCmd(uint16_t playerId, int cardsCount, netGameStateManager* gameManager, netcode::client* client)
            : clientCommand(CORE_NC_DRAW_CARDS, client), gameManager(gameManager), playerId(playerId),
              cardsCount(cardsCount)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
