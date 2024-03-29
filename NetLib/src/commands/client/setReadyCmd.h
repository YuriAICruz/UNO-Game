﻿#pragma once
#include "clientCommand.h"
#include "../../serverCommands.h"

namespace netcode
{
    class room;
}

namespace commands
{
    class NETCODE_API setReadyCmd : public clientCommand
    {
    private:
        netcode::room* room;

    public:
        setReadyCmd(netcode::room* room, netcode::client* client) :
            clientCommand(NC_ROOM_READY_STATUS, client), room(room)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
