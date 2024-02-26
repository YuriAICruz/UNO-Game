#pragma once
#include "serverCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API exitRoomServerCmd : public serverCommand
    {
    public:
        explicit exitRoomServerCmd(netcode::roomManager* manager, netcode::server* server)
            : serverCommand(NC_EXIT_ROOM, manager, server)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
        
    };
}
