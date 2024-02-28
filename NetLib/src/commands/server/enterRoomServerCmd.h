#pragma once
#include "serverCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API enterRoomServerCmd : public serverCommand
    {
    public:
        explicit enterRoomServerCmd(netcode::roomManager* manager, netcode::server* server)
            : serverCommand(NC_ENTER_ROOM, manager, server)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
    };
}
