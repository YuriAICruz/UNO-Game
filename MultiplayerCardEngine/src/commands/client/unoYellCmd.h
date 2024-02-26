#pragma once
#include "netCommands.h"
#include "commands/client/clientCommand.h"
#include "../../netGameStateManager.h"

namespace commands
{
    class NETCODE_API unoYellCmd : public clientCommand
    {
    private:
        netGameStateManager* gameManager;
        
    public:
        unoYellCmd(netGameStateManager* gameManager, netcode::client* client)
            : clientCommand(CORE_NC_YELL_UNO, client), gameManager(gameManager)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
