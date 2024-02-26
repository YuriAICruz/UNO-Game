#include "validateKeyCmd.h"

#include <sstream>

#include "../../stringUtils.h"
#include "../../client/client.h"

namespace commands
{
    bool validateKeyCmd::execute()
    {
        std::promise<bool> promise;
        std::future<bool> future = setPromise(promise);

        if (hasId)
        {
            std::stringstream ss;
            ss << authKey << NC_SEPARATOR << id;
            std::string str = ss.str();
            netClient->sendMessage(ss.str());
        }
        else
        {
            netClient->sendMessage(authKey);
        }

        return waitAndReturnPromise(future);
    }

    void validateKeyCmd::callback(const std::string& message)
    {
        auto data = stringUtils::splitString(message);
        const char* key = data[0].c_str();

        if (std::strcmp(NC_VALID_KEY, key) == 0)
        {
            pending = false;
            netClient->connected = true;
            netClient->hasId = true;
            netClient->id = std::stoi(data[1]);

            if (callbackResponse != nullptr)
            {
                callbackResponse->set_value(true);
            }
            return;
        }

        if (std::strcmp(NC_INVALID_KEY, key) == 0)
        {
            pending = false;
            if (callbackResponse != nullptr)
            {
                callbackResponse->set_value(false);
            }
            netClient->connected = false;
            netClient->close();
            return;
        }
    }

    bool validateKeyCmd::isPending(const std::string& key) const
    {
        if (!pending)
        {
            return false;
        }

        if (std::strcmp(NC_VALID_KEY, key.c_str()) == 0)
        {
            return true;
        }

        if (std::strcmp(NC_INVALID_KEY, key.c_str()) == 0)
        {
            return true;
        }

        return false;
    }
}
