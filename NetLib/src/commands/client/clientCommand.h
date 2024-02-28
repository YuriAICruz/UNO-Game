#pragma once
#include <string>

#include "../command.h"

namespace netcode
{
    class client;
}

namespace commands
{
    class NETCODE_API clientCommand : public command
    {
    protected:
        netcode::client* netClient;

    public:
        clientCommand(const char* key, netcode::client* client) : command(key), netClient(client)
        {
        }

        virtual bool execute() = 0;
        virtual void callback(const std::string& message) = 0;
    };
}
