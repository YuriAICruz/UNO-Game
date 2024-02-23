#include <sstream>

#include "enterRoomCmd.h"
#include "../../stringUtils.h"
#include "../../client/client.h"

namespace commands
{
    bool enterRoomCmd::execute()
    {
        std::promise<bool> prm;
        callbackResponse = &prm;
        auto future = prm.get_future();
        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR << roomId;
        netClient->sendMessage(ss.str());
        future.wait();
        callbackResponse = nullptr;

        return future.get();
    }

    void enterRoomCmd::callback(const std::string& message)
    {
        std::vector<std::string> data = stringUtils::splitString(message);

        if (!data.empty())
        {
            data.erase(data.begin());
        }

        netClient->setRoom(netcode::room::constructRoom(data));

        if (callbackResponse != nullptr)
        {
            callbackResponse->set_value(true);
        }
    }
}
