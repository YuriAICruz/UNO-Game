#include "getRoomsCmd.h"

#include "../../stringUtils.h"
#include "../../client/client.h"

namespace commands
{
    bool getRoomsCmd::execute()
    {
        std::promise<bool> promise;
        std::future<bool> future = setPromise(promise);

        netClient->sendMessage(cmdKey);

        return waitAndReturnPromise(future);
    }

    void getRoomsCmd::callback(const std::string& message)
    {
        auto data = stringUtils::splitString(message);
        int size = std::stoi(data[1]);
        roomList.resize(size);
        int pos = 2;

        for (int i = 0; i < size; ++i)
        {
            std::stringstream ss;
            bool first = true;
            for (int n = data.size(); pos < n; ++pos)
            {
                if (!first)
                {
                    ss << NC_SEPARATOR;
                }
                if (data[pos] == NC_OBJECT_SEPARATOR)
                {
                    pos++;
                    break;
                }
                ss << data[pos];
                first = false;
            }
            std::string str = ss.str();
            roomList[i] = netcode::room::constructRoom(str);
        }

        if (callbackResponse != nullptr)
        {
            callbackResponse->set_value(true);
        }
    }
}
