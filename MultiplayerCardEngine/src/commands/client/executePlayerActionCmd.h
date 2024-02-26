#pragma once
#include "netCommands.h"
#include "../../netGameStateManager.h"
#include "commands/client/clientCommand.h"

namespace commands
{
    class NETCODE_API executePlayerActionCmd : public clientCommand
    {
    private:
        int index;
        netGameStateManager* gameManager;

    public:
        executePlayerActionCmd(int index, netGameStateManager* manager, netcode::client* client) :
        clientCommand(CORE_NC_PLAYCARD, client), gameManager(manager), index(index)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
