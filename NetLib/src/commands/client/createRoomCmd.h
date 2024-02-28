#pragma once
#include "clientCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API createRoomCmd : public clientCommand
    {
        std::string roomName;

    public:
        createRoomCmd(const std::string& name, netcode::client* client)
            : clientCommand(NC_CREATE_ROOM, client), roomName(name)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
