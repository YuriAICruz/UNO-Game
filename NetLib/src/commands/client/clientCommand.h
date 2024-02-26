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

        bool isPending() const
        {
            return pending;
        }

        bool isPending(const std::string& key)
        {
            if (!pending)
            {
                return false;
            }

            if (std::strcmp(cmdKey, key.c_str()) == 0)
            {
                pending = false;
                return true;
            }

            return false;
        }
    };
}
