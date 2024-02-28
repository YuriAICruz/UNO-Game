#pragma once
#include "serverCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API createRoomServerCmd : public serverCommand
    {
    public:
        explicit createRoomServerCmd(netcode::roomManager* manager, netcode::server* server)
            : serverCommand(NC_CREATE_ROOM, manager, server)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
    };
}
