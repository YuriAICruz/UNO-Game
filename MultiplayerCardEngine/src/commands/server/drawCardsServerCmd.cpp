#include "drawCardsServerCmd.h"

namespace commands
{
    bool drawCardsServerCmd::execute()
    {
        return true;
    }

    bool drawCardsServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        bool canDraw = gameManager->canDrawCard();

        logger::print(
            (logger::getPrinter() << "player draw cards : " << (canDraw ? "true" : "false")
                << " drawn = [" << gameManager->getCurrentPlayerCardsDraw() << "]").str());

        if (canDraw)
        {
            uint16_t id = stoi(data[1]);
            auto amount = stoi(data[2]);

            gameManager->makePlayerDraw(gameManager->getPlayerFromId(id), amount);
        }

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR << (canDraw ? 1 : 0);
        bool result = netServer->sendMessage(ss.str(), clientSocket);

        gameManager->broadcastServerStateData(clientSocket);

        return result;
    }
}
