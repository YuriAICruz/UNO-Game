#include "hasRoomCmd.h"

#include "../../client/client.h"

namespace commands
{
    bool hasRoomCmd::execute()
    {
        netcode::room* r;
        pending = false;

        return netClient->currentRoom.count() > 0;
    }

    void hasRoomCmd::callback(const std::string& message)
    {
    }
}
