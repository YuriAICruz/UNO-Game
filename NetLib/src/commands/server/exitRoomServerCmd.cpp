#include "exitRoomServerCmd.h"

#include "../../logger.h"
#include "../../server/server.h"

namespace commands
{
    bool exitRoomServerCmd::execute()
    {
        return true;
    }

    bool exitRoomServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        logger::print("SERVER: client exiting room");

        auto client = netServer->getClient(clientSocket);
        if (nullptr == client.get())
        {
            logger::printError(
                (logger::getPrinter() << "Could not find client with socket [" << clientSocket << "]").str()
            );
        }
        else
        {
            manager->exitRoom(client.get());
        }

        return netServer->sendMessage(cmdKey, clientSocket);
    }
}
