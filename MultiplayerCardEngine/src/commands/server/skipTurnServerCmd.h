#pragma once
#include "netCommands.h"
#include "commands/server/serverCommand.h"
#include "../../netGameStateManager.h"

namespace commands
{
    class NET_ENGINE_API skipTurnServerCmd : public serverCommand
    {
    private:
        netGameStateManager* gameManager;

    public:
        explicit skipTurnServerCmd(
            netGameStateManager* gameManager, netcode::roomManager* manager, netcode::server* server)
            : serverCommand(CORE_NC_SKIP_TURN, manager, server), gameManager(gameManager)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
    };
}
