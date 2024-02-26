#pragma once
#include "serverCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API setSeedServerCmd : public serverCommand
    {
    public:
        explicit setSeedServerCmd(netcode::roomManager* manager, netcode::server* server)
            : serverCommand(NC_SET_SEED, manager, server)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
        
    };
}
