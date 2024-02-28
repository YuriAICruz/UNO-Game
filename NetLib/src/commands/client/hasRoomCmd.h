#pragma once
#include "clientCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API hasRoomCmd : public clientCommand
    {
    public:
        hasRoomCmd(netcode::client* client)
            : clientCommand(NC_NO_CALLBACK, client)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
