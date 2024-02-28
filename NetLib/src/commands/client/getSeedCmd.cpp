#include "getSeedCmd.h"

#include "../../client/client.h"

namespace commands
{
    bool getSeedCmd::execute()
    {
        pending = false;
        seed = netClient->seed;

        return true;
    }

    void getSeedCmd::callback(const std::string& message)
    {
    }
}
