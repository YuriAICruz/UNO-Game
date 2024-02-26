#include "getRoomServerCmd.h"

#include "../../server/roomManager.h"

namespace commands
{
    bool getRoomServerCmd::execute()
    {
        return true;
    }

    bool getRoomServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        uint16_t id = std::stoi(data[1]);
        return manager->getRoom(id);
    }
}
