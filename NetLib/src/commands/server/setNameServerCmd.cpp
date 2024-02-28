#include "setNameServerCmd.h"

#include "../../logger.h"
#include "../../server/server.h"

namespace commands
{
    bool setNameServerCmd::execute()
    {
        return true;
    }

    bool setNameServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        logger::print("SERVER: updating client name");

        auto client = netServer->getClient(clientSocket);
        client->name = data[1];

        return netServer->sendMessage(NC_SET_NAME, clientSocket);
    }
}
