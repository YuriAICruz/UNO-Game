#pragma once
#include "serverCommands.h"
#include "../../netGameStateManager.h"
#include "commands/server/serverCommand.h"

namespace commands
{
    class NET_ENGINE_API syncVarServerCmd : public serverCommand
    {
    private:
        netGameStateManager* gameManager;
        
    public:
        explicit syncVarServerCmd(netGameStateManager* gameManager, netcode::roomManager* manager, netcode::server* server)
            : serverCommand(NC_SYNC_VAR, manager, server), gameManager(gameManager)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
        
    };
}
