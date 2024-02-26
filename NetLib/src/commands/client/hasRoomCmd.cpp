#include "hasRoomCmd.h"

#include "getRoomCmd.h"
#include "../../client/client.h"

namespace commands
{
    bool hasRoomCmd::execute()
    {
        netcode::room* r;
        pending = false;

        if (netClient->executeCommand<getRoomCmd>(r))
        {
            return r->count() > 0;
        }

        return false;
    }

    void hasRoomCmd::callback(const std::string& message)
    {
    }
}
