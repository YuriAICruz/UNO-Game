#pragma once
#include "clientCommand.h"

namespace commands
{
    class NETCODE_API enterRoomCmd : public clientCommand
    {
        int roomId;

    public:
        enterRoomCmd(int roomId, const char* key, netcode::client* client) : clientCommand(key, client), roomId(roomId)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
