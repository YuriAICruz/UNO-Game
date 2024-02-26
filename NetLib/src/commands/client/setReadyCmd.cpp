#include "setReadyCmd.h"

#include "sendRoomReadyStatusCmd.h"
#include "../../client/client.h"

namespace commands
{
    bool setReadyCmd::execute()
    {
        bool result = netClient->executeCommand<sendRoomReadyStatusCmd>(room, true);

        if (result && netClient->onRoomReady != nullptr)
        {
            netClient->onRoomReady(true);
        }
        pending = false;

        return result;
    }

    void setReadyCmd::callback(const std::string& message)
    {
    }
}
