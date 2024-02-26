#include "createRoomCmd.h"
#include "../../stringUtils.h"
#include "../../client/client.h"

namespace commands
{
    bool createRoomCmd::execute()
    {
        std::promise<bool> promise;
        std::future<bool> future = setPromise(promise);

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR << roomName;
        netClient->sendMessage(ss.str());

        return waitAndReturnPromise(future);
    }

    void createRoomCmd::callback(const std::string& message)
    {
        std::vector<std::string> data = stringUtils::splitString(message);

        pending = false;
        if (!data.empty())
        {
            data.erase(data.begin());
        }else
        {
            if (callbackResponse != nullptr)
            {
                callbackResponse->set_value(false);
            }
            return;
        }

        netClient->setRoom(netcode::room::constructRoom(data));

        if (callbackResponse != nullptr)
        {
            callbackResponse->set_value(true);
        }
    }
}
