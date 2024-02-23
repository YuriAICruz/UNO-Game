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
        const char* cmdKey;
        bool pending = true;

    public:
        clientCommand(const char* key, netcode::client* client) : netClient(client), cmdKey(key)
        {
        }

        virtual bool execute() = 0;
        virtual void callback(const std::string& message) = 0;

        bool isPending(const std::string& key)
        {
            if(!pending)
            {
                return false;
            }

            pending = false;
            return std::strcmp(cmdKey, key.c_str()) == 0;
        }
    };
}
