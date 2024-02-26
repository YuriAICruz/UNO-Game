#include "gameSettingsCmd.h"

namespace commands
{
    bool gameSettingsCmd::execute()
    {
        std::promise<bool> promise;
        auto future = setPromise(promise);

        std::string str = gameManager->encryptGameSettings(path, cmdKey);
        netClient->sendMessage(str.c_str());
        return waitAndReturnPromise(future);
    }

    void gameSettingsCmd::callback(const std::string& message)
    {
        gameManager->decryptGameSettingsAndSetup(message);

        pending = false;
        if (callbackResponse != nullptr)
        {
            callbackResponse->set_value(true);
        }
    }
}
