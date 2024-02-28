#include "updateRoomStatusServerCmd.h"

#include <sstream>
#include "../../server/server.h"

namespace commands
{
    bool updateRoomStatusServerCmd::execute()
    {
        return true;
    }

    bool updateRoomStatusServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        bool ready = data[1] == "1";

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR;

        auto client = netServer->getClient(clientSocket).get();

        auto room = roomManager->getRoom(client);
        if (room == nullptr || room->isLocked())
        {
            ss << 0;
            netServer->broadcastToRoom(ss.str(), clientSocket);
            return false;
        }

        client->ready = ready;

        ss << 1;
        netServer->broadcastToRoom(ss.str(), clientSocket);

        if (roomManager->roomClientsAreReady(room->getId()))
        {
            netServer->broadcastToRoom(NC_ROOM_ALL_READY, clientSocket);
        }
        else
        {
            netServer->broadcastToRoom(NC_ROOM_NOT_READY, clientSocket);
        }
        return true;
    }
}
