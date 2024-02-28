#include "executePlayerActionServerCmd.h"

namespace commands{
    bool executePlayerActionServerCmd::execute()
    {
        return true;
    }

    bool executePlayerActionServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {        
        logger::print("received player action");
        int index = std::stoi(data[1]);
        turnSystem::IPlayer* currentPlayer = gameManager->getCurrentPlayer();

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR;

        auto card = currentPlayer->pickCard(index);
        if (gameManager->tryExecutePlayerAction(card))
        {
            logger::print("player action executed successful");
            gameManager->broadcastServerStateData(clientSocket);
            if (gameManager->isGameRunning())
            {
                ss << 1;
            }
            else
            {
                ss << 2;
            }
        }
        else
        {
            logger::print("player action not executed");
            currentPlayer->receiveCard(card);
            ss << 0;
        }
        return netServer->sendMessage(ss.str(), clientSocket);
    }
}
