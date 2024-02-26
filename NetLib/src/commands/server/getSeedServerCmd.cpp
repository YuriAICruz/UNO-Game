#include "getSeedServerCmd.h"

#include "../../logger.h"
#include "../../server/server.h"

namespace commands
{
    bool getSeedServerCmd::execute()
    {
        return true;
    }

    bool getSeedServerCmd::callback(std::vector<std::string>& data, SOCKET clientSocket)
    {
        logger::print("SERVER: getting seed");
        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR << netServer->getSeed();
        std::string str = ss.str();
        const char* responseData = str.c_str();
        return netServer->sendMessage(responseData, clientSocket);
    }
}
