﻿#include "setNotReadyCmd.h"

#include "sendRoomReadyStatusCmd.h"
#include "../../client/client.h"

namespace commands
{
    bool setNotReadyCmd::execute()
    {
        bool result = netClient->executeCommand<sendRoomReadyStatusCmd>(room, false);
        
        pending = false;

        return result;
    }

    void setNotReadyCmd::callback(const std::string& message)
    {
        if (callbackOnly)
        {
            netClient->roomReady(false);
        }
    }
}
