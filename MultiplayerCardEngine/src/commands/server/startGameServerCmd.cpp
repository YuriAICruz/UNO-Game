#include "startGameServerCmd.h"

#include "commands/server/lockRoomServerCmd.h"

namespace commands
{
    bool startGameServerCmd::execute()
    {
        return true;
    }

    bool startGameServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        if (!gameManager->isGameRunning())
        {
            gameManager->startGame();
        }

        netServer->executeServerCommand<commands::lockRoomServerCmd>(clientSocket);
        netServer->broadcastToRoom(cmdKey, clientSocket);
        gameManager->broadcastServerStateData(clientSocket);
        return true;
    }
}
