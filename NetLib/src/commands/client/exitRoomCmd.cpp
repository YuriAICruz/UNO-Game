#include "exitRoomCmd.h"
#include "../../client/client.h"

namespace commands
{
    bool exitRoomCmd::execute()
    {
        std::promise<bool> promise;
        std::future<bool> future = setPromise(promise);

        netClient->sendMessage(cmdKey);

        return waitAndReturnPromise(future);
    }

    void exitRoomCmd::callback(const std::string& message)
    {
        netClient->setRoom(netcode::room());

        setCallback(true);
    }
}
