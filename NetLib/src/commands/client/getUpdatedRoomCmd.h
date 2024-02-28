#pragma once
#include "clientCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API getUpdatedRoomCmd : public clientCommand
    {
    private:
        uint16_t id;

    public:
        getUpdatedRoomCmd(uint16_t id, netcode::client* client)
            : clientCommand(NC_GET_ROOM, client), id(id)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
