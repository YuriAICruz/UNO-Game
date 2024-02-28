#include "getRoomServerCmd.h"

#include "../../server/roomManager.h"
#include "../../server/server.h"

namespace commands
{
    bool getRoomServerCmd::execute()
    {
        return true;
    }

    bool getRoomServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        logger::print((logger::printer() << "SERVER: getting room [" << data[1] << "] information").str());
        int id = stoi(data[1]);

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR;
        ss << roomManager->getRoomSerialized(id);

        logger::print((logger::getPrinter() << "SERVER: sent to client room updated data [" << id << "]").str());
        return netServer->sendMessage(ss.str(), clientSocket);
    }
}
