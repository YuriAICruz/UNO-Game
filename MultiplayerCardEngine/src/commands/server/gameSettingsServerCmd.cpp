#include "gameSettingsServerCmd.h"

#include "logger.h"

namespace commands
{
    bool gameSettingsServerCmd::execute()
    {
        return true;
    }

    bool gameSettingsServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        logger::print("SERVER: Updating Game Settings");
        gameManager->decryptGameSettingsAndSetup(data);
        logger::print("SERVER: Game Settings Updated");

        netServer->broadcastUpdatedRoom(clientSocket);

        std::string path = data[2];
        std::string str = gameManager->encryptGameSettings(path, cmdKey);
        return netServer->broadcastToRoom(str, clientSocket);
    }
}
