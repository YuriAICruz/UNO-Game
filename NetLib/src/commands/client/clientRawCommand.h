#pragma once
#include "clientCommand.h"

namespace netcode
{
    class client;
}

namespace commands
{
    class clientRawCommand : public clientCommand
    {
    public:
        clientRawCommand(const char* key, netcode::client* client) : clientCommand(key, client)
        {
        }

        bool acceptRaw() override
        {
            return true;
        }

        virtual void rawCallback(char* rawStr, int strSize) = 0;
    };
}
