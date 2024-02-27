#pragma once
#include "commands/client/clientCommand.h"
#include "netCommands.h"
#include "../../netGameStateManager.h"

namespace commands
{
    class NET_ENGINE_API gameSettingsCmd : public clientCommand
    {
    private:
        netGameStateManager* gameManager;
        std::string path;

    public:
        gameSettingsCmd(const std::string& path, netGameStateManager* gameManager, netcode::client* client)
            : clientCommand(CORE_NC_GAME_SETTINGS, client), gameManager(gameManager), path(path)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
