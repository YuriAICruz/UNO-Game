#pragma once
#include <future>
#include "../../framework.h"

namespace commands
{
    class NETCODE_API command
    {
    protected:
        std::promise<bool>* callbackResponse = nullptr;
    public:
        virtual ~command() = default;
    };
}
