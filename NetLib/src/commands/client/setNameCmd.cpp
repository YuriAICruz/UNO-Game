#include "setNameCmd.h"
#include "../../client/client.h"

namespace commands
{
    bool setNameCmd::execute()
    {
        netClient->clientName = name;

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR << name;
        netClient->sendMessage(ss.str());
        pending = false;

        return true;
    }

    void setNameCmd::callback(const std::string& message)
    {
    }
}
