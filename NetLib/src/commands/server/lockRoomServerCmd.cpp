#include "lockRoomServerCmd.h"
#include "../../server/server.h"

namespace commands
{
    bool lockRoomServerCmd::execute()
    {
        manager->getRoom(netServer->getClient(clientSocket).get())->lock();
        return true;
    }

    bool lockRoomServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        return false;
    }
}
