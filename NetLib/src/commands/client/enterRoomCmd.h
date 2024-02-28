#pragma once
#include "clientCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API enterRoomCmd : public clientCommand
    {
        int roomId;

    public:
        enterRoomCmd(int roomId, netcode::client* client) : clientCommand(NC_ENTER_ROOM, client), roomId(roomId)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
