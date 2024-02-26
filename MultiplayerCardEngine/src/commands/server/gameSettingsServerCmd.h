#pragma once
#include "netCommands.h"
#include "../../netGameStateManager.h"
#include "commands/server/serverCommand.h"

namespace commands
{
    class NETCODE_API gameSettingsServerCmd : public serverCommand
    {
    private:
        netGameStateManager* gameManager;

    public:
        explicit gameSettingsServerCmd(
            netGameStateManager* gameManager, netcode::roomManager* manager, netcode::server* server)
            : serverCommand(CORE_NC_GAME_SETTINGS, manager, server), gameManager(gameManager)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
    };
}
