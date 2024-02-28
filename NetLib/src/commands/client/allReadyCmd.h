#pragma once
#include "clientCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API allReadyCmd : public clientCommand
    {
    public:
        allReadyCmd(const char* key, netcode::client* client)
            : clientCommand(NC_ROOM_ALL_READY, client)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
        
    };
}
