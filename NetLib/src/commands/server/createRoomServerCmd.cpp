#include "createRoomServerCmd.h"

#include "../../logger.h"
#include "../../stringUtils.h"
#include "../../server/roomManager.h"
#include "../../server/server.h"

namespace commands
{
    bool createRoomServerCmd::execute()
    {
        return true;
    }

    bool createRoomServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        logger::print((logger::printer() << "SERVER: creating room [" << data[1] << "]").str());

        int roomsCount = manager->roomsCount();
        int id = roomsCount;
        manager->createRoom(id, data[1]);
        manager->enterRoom(id, netServer->getClient(clientSocket));

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR;
        ss << manager->getRoomSerialized(id);

        bool result = netServer->sendMessage(ss.str(), clientSocket);

        logger::print((logger::getPrinter() << "SERVER: created room with id" << id << "").str());

        if (netServer->onRoomCreated != nullptr)
        {
            netServer->onRoomCreated(manager->getRoom(id));
        }

        return result;
    }
}
