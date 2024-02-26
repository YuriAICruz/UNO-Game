#pragma once
#include "clientCommand.h"
#include "../../serverCommands.h"
#include "../../server/room.h"

namespace commands
{
    class NETCODE_API getRoomsCmd : public clientCommand
    {
    private:
        std::vector<netcode::room>& roomList;

    public:
        getRoomsCmd(std::vector<netcode::room>& roomList, netcode::client* client)
            : clientCommand(NC_LIST_ROOMS, client), roomList(roomList)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
