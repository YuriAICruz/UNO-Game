#pragma once
#include "serverCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API updateRoomStatusServerCmd : public serverCommand
    {
    public:
        explicit updateRoomStatusServerCmd(netcode::roomManager* manager, netcode::server* server)
            : serverCommand(NC_ROOM_READY_STATUS, manager, server)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
    };
}
