#include "getRoomCmd.h"

#include "../../client/client.h"

namespace commands
{
    bool getRoomCmd::execute()
    {
        roomRef = &netClient->currentRoom;
        pending = false;
        return true;
    }

    void getRoomCmd::callback(const std::string& message)
    {
    }
}
