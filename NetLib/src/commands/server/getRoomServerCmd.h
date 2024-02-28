#pragma once
#include "serverCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API getRoomServerCmd : public serverCommand
    {
    public:
        explicit getRoomServerCmd(netcode::roomManager* manager, netcode::server* server)
            : serverCommand(NC_GET_ROOM, manager, server)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
    };
}
