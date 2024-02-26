#pragma once
#include "serverCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API getRoomsServerCmd : public serverCommand
    {
    public:
        explicit getRoomsServerCmd(netcode::roomManager* manager, netcode::server* server)
            : serverCommand(NC_LIST_ROOMS, manager, server)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
        
    };
}
