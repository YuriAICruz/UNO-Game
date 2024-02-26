#include "unoYellServerCmd.h"

namespace commands
{
    bool unoYellServerCmd::execute()
    {
        return true;
    }

    bool unoYellServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        bool canYell = gameManager->getCurrentPlayer()->getHand().size() == 2;

        if (canYell)
        {
            gameManager->yellUno();
        }

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR << (canYell ? "1" : "0");

        return netServer->broadcastToRoom(ss.str(), clientSocket);
    }
}
