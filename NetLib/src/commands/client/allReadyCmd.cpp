#include "allReadyCmd.h"
#include "../../client/client.h"

namespace commands
{
    bool allReadyCmd::execute()
    {
        return true;
    }

    void allReadyCmd::callback(const std::string& message)
    {
        if (callbackOnly)
        {
            netClient->roomReady(true);
        }
    }
}
