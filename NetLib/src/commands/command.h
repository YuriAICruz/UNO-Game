#pragma once
#include <future>
#include "../../framework.h"

namespace commands
{
    class NETCODE_API command
    {
    protected:
        std::promise<bool>* callbackResponse = nullptr;
        bool pending = true;
        const char* cmdKey;

    public:
        command(const char* cmd_key) : cmdKey(cmd_key)
        {
        }

        virtual ~command() = default;

        virtual bool isPending()
        {
            return pending;
        }

        virtual bool isPending(const std::string& key) const
        {
            if (!pending)
            {
                return false;
            }

            if (std::strcmp(cmdKey, key.c_str()) == 0)
            {
                return true;
            }

            return false;
        }

    protected:
        std::future<bool> setPromise(std::promise<bool>& promise);
        bool waitAndReturnPromise(std::future<bool>& future);
    };
}
