#include "getRoomsServerCmd.h"

#include "../../logger.h"
#include "../../server/server.h"

namespace commands
{
    bool getRoomsServerCmd::execute()
    {
        return true;
    }

    bool getRoomsServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        logger::print("SERVER: listing rooms");
        std::string str = roomManager->listRooms(cmdKey);
        return netServer->sendMessage(str, clientSocket);
    }
}
