#include "getUpdatedRoomCmd.h"

#include <sstream>

#include "../../serverCommands.h"
#include "../../stringUtils.h"
#include "../../client/client.h"

namespace commands
{
    bool getUpdatedRoomCmd::execute()
    {
        std::promise<bool> promise;
        std::future<bool> future = setPromise(promise);

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR << id;
        netClient->sendMessage(ss.str());

        return waitAndReturnPromise(future);
    }

    void getUpdatedRoomCmd::callback(const std::string& message)
    {
        std::vector<std::string> data = stringUtils::splitString(message);

        if (!data.empty())
        {
            data.erase(data.begin());
        }else
        {
            setCallback(false);
            return;
        }

        netcode::room r;
        if(netcode::room::constructRoom(data, r))
        {
            netClient->setRoom(r);
        }

        setCallback(true);
    }
}
