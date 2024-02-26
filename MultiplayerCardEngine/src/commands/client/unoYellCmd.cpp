#include "unoYellCmd.h"

#include "stringUtils.h"

namespace commands
{
    bool unoYellCmd::execute()
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

    void unoYellCmd::callback(const std::string& message)
    {
        auto data = stringUtils::splitString(message);

        bool canYell = data[1] == "1";
        if (callbackResponse != nullptr)
        {
            callbackResponse->set_value(canYell);
        }

        if (canYell)
        {
            gameManager->yellUno();
        }
    }
}
