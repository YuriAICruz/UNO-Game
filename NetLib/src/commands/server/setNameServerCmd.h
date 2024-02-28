#pragma once
#include "serverCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API setNameServerCmd : public serverCommand
    {
    public:
        explicit setNameServerCmd(netcode::roomManager* manager, netcode::server* server)
            : serverCommand(NC_SET_NAME, manager, server)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
        
    };
}
