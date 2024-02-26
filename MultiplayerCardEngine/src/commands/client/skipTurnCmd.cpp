#include "skipTurnCmd.h"

#include "stringUtils.h"

namespace commands
{
    bool skipTurnCmd::execute()
    {
        if (!gameManager->isCurrentPlayer())
        {
            return false;
        }

        std::promise<bool> promise;
        auto future = setPromise(promise);
        netClient->sendMessage(cmdKey);

        return waitAndReturnPromise(future);
    }

    void skipTurnCmd::callback(const std::string& message)
    {
        if (callbackResponse != nullptr)
        {
            std::vector<std::string> data = stringUtils::splitString(message);
            callbackResponse->set_value(data[1] == "1");
        }
    }
}
