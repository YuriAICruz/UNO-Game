#pragma once
#include <string>

#include "../command.h"
#include "../../winSockImp.h"

namespace netcode
{
    class server;
    class roomManager;
}

namespace commands
{
    class serverCommand : public command
    {
    protected:
        netcode::server* netServer;
        netcode::roomManager* roomManager;

    public:
        serverCommand(const char* key, netcode::roomManager* manager, netcode::server* server) :
            command(key), netServer(server), roomManager(manager)
        {
        }

        virtual bool execute() = 0;
        virtual bool callback(std::vector<std::string>& data, SOCKET clientSocket) = 0;
    };
}
