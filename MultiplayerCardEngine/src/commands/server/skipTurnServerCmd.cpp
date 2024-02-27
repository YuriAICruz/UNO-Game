#include "skipTurnServerCmd.h"

namespace commands
{
    bool skipTurnServerCmd::execute()
    {
        return true;
    }

    bool skipTurnServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        bool canSkip = gameManager->canSkipTurn();

        if (canSkip)
        {
            gameManager->skipTurn();
        }

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR << (canSkip ? 1 : 0);
        bool result = netServer->sendMessage(ss.str(), clientSocket);

        gameManager->broadcastServerStateData(clientSocket);

        return result;
    }
}
