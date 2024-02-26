#include "executePlayerActionCmd.h"

#include "logger.h"
#include "stringUtils.h"

namespace commands
{
    bool executePlayerActionCmd::execute()
    {
        if (!gameManager->isCurrentPlayer())
        {
            return false;
        }

        std::promise<bool> promise;
        auto future = setPromise(promise);

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR << index;
        std::string str = ss.str();

        netClient->sendMessage(str.c_str());
        return waitAndReturnPromise(future);
    }

    void executePlayerActionCmd::callback(const std::string& message)
    {
        std::vector<std::string> data = stringUtils::splitString(message);

        pending = false;
        if (callbackResponse != nullptr)
        {
            int r = stoi(data[1]);
            callbackResponse->set_value(r > 0);

            // server can respond 2 if the game has ended in this turn
            if (r == 2)
            {
                netClient->showClientEndGame(message);
            }
        }
    }
}
