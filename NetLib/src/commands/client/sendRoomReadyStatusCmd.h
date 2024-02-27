#pragma once
#include "clientCommand.h"
#include "../../serverCommands.h"
#include "../../server/room.h"

namespace commands
{
    class sendRoomReadyStatusCmd : public clientCommand
    {
    private:
        netcode::room* room;
        bool ready;

    public:
        sendRoomReadyStatusCmd(netcode::room* room, bool ready, netcode::client* client)
            : clientCommand(NC_ROOM_READY_STATUS, client), room(room), ready(ready)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
