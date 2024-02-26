#include "setSeedServerCmd.h"

#include "../../server/server.h"

namespace commands
{
    bool setSeedServerCmd::execute()
    {
        return true;
    }

    bool setSeedServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        netServer->setSeed(std::stoull(data[1]));
        return true;
    }
}
