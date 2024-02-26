#pragma once
#include "netCommands.h"
#include "commands/server/serverCommand.h"
#include "../../netGameStateManager.h"

namespace commands
{
    class NET_ENGINE_API unoYellServerCmd : public serverCommand
    {
    private:
        netGameStateManager* gameManager;
        
    public:
        explicit unoYellServerCmd(netGameStateManager* gameManager, netcode::roomManager* manager, netcode::server* server)
            : serverCommand(CORE_NC_YELL_UNO, manager, server), gameManager(gameManager)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
        
    };
}
