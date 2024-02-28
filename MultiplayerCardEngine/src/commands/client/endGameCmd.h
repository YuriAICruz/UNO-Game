#pragma once
#include "netCommands.h"
#include "commands/client/clientCommand.h"
#include "../../netGameStateManager.h"

namespace commands
{
    class NET_ENGINE_API endGameCmd : public clientCommand
    {
    private:
        netGameStateManager* gameManager;

    public:
        endGameCmd(netGameStateManager* gameManager, netcode::client* client)
            : clientCommand(CORE_NC_GAME_END, client), gameManager(gameManager)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
