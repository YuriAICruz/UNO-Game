#pragma once
#include "clientCommand.h"
#include "../../serverCommands.h"

namespace netcode
{
    class room;
}

namespace commands
{
    class NETCODE_API getRoomCmd : public clientCommand
    {
        netcode::room*& roomRef;

    public:
        getRoomCmd(netcode::room*& room, netcode::client* client)
            : clientCommand(NC_NO_CALLBACK, client), roomRef(room)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
