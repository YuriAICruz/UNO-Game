#pragma once
#include "serverCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API getSeedServerCmd : public serverCommand
    {
    public:
        explicit getSeedServerCmd(netcode::roomManager* manager, netcode::server* server)
            : serverCommand(NC_GET_SEED, manager, server)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
    };
}
