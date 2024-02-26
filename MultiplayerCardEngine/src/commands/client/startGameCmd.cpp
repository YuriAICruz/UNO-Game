﻿#include "startGameCmd.h"

namespace commands
{
    bool startGameCmd::execute()
    {
        std::promise<bool> promise;
        std::future<bool> future = setPromise(promise);

        netClient->sendMessage(cmdKey);

        return waitAndReturnPromise(future);
    }

    void startGameCmd::callback(const std::string& message)
    {
        if (!gameManager->isGameRunning())
        {
            gameManager->startGame();
        }

        pending = false;
        if (callbackResponse != nullptr)
        {
            callbackResponse->set_value(true);
        }
    }
}
