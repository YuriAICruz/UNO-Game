#include "syncVarServerCmd.h"

namespace commands
{
    bool syncVarServerCmd::execute()
    {
        return true;
    }

    bool syncVarServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        int id = stoi(data[1]);
        int value = stoi(data[2]);

        gameManager->updateVarsDictionary(id, value);

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR << id << NC_SEPARATOR << value;
        return netServer->broadcastToRoom(ss.str(), clientSocket);
    }
}
