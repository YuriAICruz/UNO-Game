﻿#pragma once
#include "netCommands.h"
#include "commands/client/clientRawCommand.h"
#include "../../netGameStateManager.h"

namespace commands
{
    class NETCODE_API updateStateCmd : public clientRawCommand
    {
    private:
        netGameStateManager* gameManager;
        std::function<void()> onUpdated = nullptr;

    public:
        updateStateCmd(std::function<void()> onUpdated, netGameStateManager* gameManager, netcode::client* client)
            : clientRawCommand(CORE_NC_UPDATE_STATE, client), gameManager(gameManager), onUpdated(onUpdated)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
        void rawCallback(char* rawStr, int strSize) override;
    };
}
