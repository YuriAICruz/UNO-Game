#pragma once
#include "netCommands.h"
#include "../../netGameStateManager.h"
#include "commands/client/clientCommand.h"

namespace commands
{
    class NET_ENGINE_API startGameCmd : public clientCommand
    {
    private:
        netGameStateManager* gameManager;

    public:
        startGameCmd(netGameStateManager* gameManager, netcode::client* client)
            : clientCommand(CORE_NC_GAME_START, client), gameManager(gameManager)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
