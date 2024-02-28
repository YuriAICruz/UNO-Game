#pragma once
#include "netCommands.h"
#include "commands/client/clientCommand.h"
#include "../../netGameStateManager.h"

namespace commands
{
    class NET_ENGINE_API skipTurnCmd : public clientCommand
    {
    private:
        netGameStateManager* gameManager;

    public:
        skipTurnCmd(netGameStateManager* gameManager, netcode::client* client)
            : clientCommand(CORE_NC_SKIP_TURN, client), gameManager(gameManager)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
