#include "unlockRoomServerCmd.h"
#include "../../server/server.h"

namespace commands
{
    bool unlockRoomServerCmd::execute()
    {
        manager->getRoom(netServer->getClient(clientSocket).get())->unlock();
        return true;
    }

    bool unlockRoomServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        return false;
    }
}
