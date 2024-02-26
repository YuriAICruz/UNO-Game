#include "sendRoomReadyStatusCmd.h"

#include "../../stringUtils.h"
#include "../../client/client.h"

namespace commands
{
    bool sendRoomReadyStatusCmd::execute()
    {
        std::promise<bool> promise;
        std::future<bool> future = setPromise(promise);
        
        bool lastState = room->isClientReady();

        if (ready)
        {
            room->setClientReady();
        }
        else
        {
            room->setClientNotReady();
        }

        std::stringstream ss;
        ss << NC_ROOM_READY_STATUS << NC_SEPARATOR << (ready ? 1 : 0);
        netClient->sendMessage(ss.str());

        bool result = waitAndReturnPromise(future);

        if (!result)
        {
            if (lastState)
            {
                room->setClientReady();
            }
            else
            {
                room->setClientNotReady();
            }
        }

        return result;
    }

    void sendRoomReadyStatusCmd::callback(const std::string& message)
    {
        auto data = stringUtils::splitString(message);

        bool success = data[1] == "1";
        if (callbackResponse != nullptr)
        {
            callbackResponse->set_value(success);
        }
    }
}
