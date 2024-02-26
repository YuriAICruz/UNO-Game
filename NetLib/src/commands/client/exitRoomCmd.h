#pragma once
#include "clientCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API exitRoomCmd : public clientCommand
    {
    public:
        exitRoomCmd(netcode::client* client)
            : clientCommand(NC_EXIT_ROOM, client)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
