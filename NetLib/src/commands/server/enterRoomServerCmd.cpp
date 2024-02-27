#include "enterRoomServerCmd.h"

#include "../../logger.h"
#include "../../server/server.h"
#include "../../server/roomManager.h"

namespace commands
{
    class sendToClientServerCmd;

    bool enterRoomServerCmd::execute()
    {
        return true;
    }

    bool enterRoomServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        logger::print((logger::printer() << "SERVER: client entering room [" << data[1] << "]").str());

        int id = stoi(data[1]);
        std::stringstream ss;
        ss << cmdKey;

        if (roomManager->enterRoom(id, netServer->getClient(clientSocket)))
        {
            ss << NC_SEPARATOR;
            ss << roomManager->getRoomSerialized(id);

            logger::print((logger::getPrinter() << "SERVER: added client to room with id [" << id << "]").str());
            return netServer->broadcastToRoom(ss.str(), clientSocket);
        }

        logger::print((logger::getPrinter() << "SERVER: Could not add client to room [" << id << "]").str());
        bool result = netServer->sendMessage(ss.str(), clientSocket);
        return false;
    }
}
