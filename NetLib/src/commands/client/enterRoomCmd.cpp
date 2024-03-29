﻿#include <sstream>

#include "enterRoomCmd.h"
#include "../../stringUtils.h"
#include "../../client/client.h"

namespace commands
{
    bool enterRoomCmd::execute()
    {
        std::promise<bool> promise;
        std::future<bool> future = setPromise(promise);

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR << roomId;
        netClient->sendMessage(ss.str());

        return waitAndReturnPromise(future);
    }

    void enterRoomCmd::callback(const std::string& message)
    {
        std::vector<std::string> data = stringUtils::splitString(message);

        if (data.size() == 1)
        {
            setCallback(false);
            return;
        }

        if (!data.empty())
        {
            data.erase(data.begin());
        }

        netcode::room r;
        if(netcode::room::constructRoom(data, r))
        {
            netClient->setRoom(r);
        }

        setCallback(true);
    }
}
