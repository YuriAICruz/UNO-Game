#pragma once
#include <future>
#include "../../framework.h"

namespace commands
{
    class NETCODE_API command
    {
    protected:
        std::promise<bool>* callbackResponse = nullptr;

        std::future<bool> setPromise(std::promise<bool>& promise);
        std::future<bool> createPromise();
        bool waitAndReturnPromise(std::future<bool>& future);

    public:
        virtual ~command() = default;
    };
}
