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

    void command::setCallback(bool value)
    {
        if (callbackOnly) return;

        pending = false;
        if (callbackResponse != nullptr)
        {
            callbackResponse->set_value(value);
        }
    }
}
