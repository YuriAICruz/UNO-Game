#pragma once
#include <string>

#include "../command.h"
#include "../../winSockImp.h"
#include "../../server/server.h"

namespace commands
{
    class serverCommand : command
    {
    protected:
        netcode::server* netServer;

    public:
        serverCommand(netcode::server* server) : netServer(server)
        {
        }

        virtual bool execute() = 0;
        virtual void callback(std::string& message, SOCKET clientConnection) = 0;
    };
}
