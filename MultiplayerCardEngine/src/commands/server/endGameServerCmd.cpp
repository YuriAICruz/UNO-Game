#include "endGameServerCmd.h"

namespace commands
{
    bool endGameServerCmd::execute()
    {
        return true;
    }

    bool endGameServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        gameManager->endGame();

        SOCKET sc = gameManager->getServerRoom()->clients()[0]->connection;

        if (!gameManager->isInRoom(sc))
        {
            return false;
        }

        netServer->broadcastToRoom(cmdKey, sc);

        gameManager->getServerRoom()->unlock();
        return true;
    }
}
