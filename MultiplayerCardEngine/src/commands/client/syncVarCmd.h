#pragma once
#include "commands/client/clientCommand.h"
#include "../../netGameStateManager.h"
#include "serverCommands.h"

namespace commands
{
    class NET_ENGINE_API syncVarCmd : public clientCommand
    {
    private:
        int id;
        int value;
        netGameStateManager* gameManager;

    public:
        syncVarCmd(int id, int value, netGameStateManager* gameManager, netcode::client* client)
            : clientCommand(NC_SYNC_VAR, client), gameManager(gameManager), id(id), value(value)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
