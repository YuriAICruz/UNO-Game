﻿#pragma once
#include "serverCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API unlockRoomServerCmd : public serverCommand
    {
        SOCKET clientSocket;

    public:
        explicit unlockRoomServerCmd(SOCKET cs, netcode::roomManager* manager, netcode::server* server)
            : serverCommand(NC_NO_CALLBACK, manager, server), clientSocket(cs)
        {
        }

        bool execute() override;
        bool callback(std::vector<std::string>& data, SOCKET clientSocket) override;
    };
}
