#include "drawCardsCmd.h"

#include "stringUtils.h"

namespace commands
{
    bool drawCardsCmd::execute()
    {
        if (!gameManager->isCurrentPlayer())
        {
            return false;
        }

        std::promise<bool> promise;
        auto future = setPromise(promise);

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR << playerId << NC_SEPARATOR << cardsCount;
        std::string str = ss.str();
        netClient->sendMessage(str.c_str());

        return waitAndReturnPromise(future);
    }

    void drawCardsCmd::callback(const std::string& message)
    {
        if (callbackResponse != nullptr)
        {
            std::vector<std::string> data = stringUtils::splitString(message);
            callbackResponse->set_value(data[1] == "1");
        }
    }
}
