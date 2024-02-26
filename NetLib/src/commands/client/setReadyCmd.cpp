#include "setReadyCmd.h"

#include "sendRoomReadyStatusCmd.h"
#include "../../client/client.h"

namespace commands
{
    bool setReadyCmd::execute()
    {
        bool result = netClient->executeCommand<sendRoomReadyStatusCmd>(room, true);
        pending = false;

        if (result && netClient->onRoomReady != nullptr)
        {
            netClient->onRoomReady(true);
        }

        return result;
    }

    void setReadyCmd::callback(const std::string& message)
    {
    }
}
