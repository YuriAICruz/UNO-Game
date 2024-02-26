#pragma once
#include "netCommands.h"
#include "commands/server/serverCommand.h"
#include "../../netGameStateManager.h"

namespace commands
{
    class NETCODE_API drawCardsServerCmd : public serverCommand
    {
    private:
        netGameStateManager* gameManager;
        
    public:
        explicit drawCardsServerCmd(netGameStateManager* gameManager, netcode::roomManager* manager, netcode::server* server)
            : serverCommand(CORE_NC_DRAW_CARDS, manager, server), gameManager(gameManager)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
        
    };
}
