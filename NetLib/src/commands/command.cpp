#include "command.h"

namespace commands
{
    std::future<bool> command::setPromise(std::promise<bool>& promise)
    {
        callbackResponse = &promise;
        return promise.get_future();
    }

    bool command::waitAndReturnPromise(std::future<bool>& future)
    {
        future.wait();
        callbackResponse = nullptr;

        return future.get();
    }
}
