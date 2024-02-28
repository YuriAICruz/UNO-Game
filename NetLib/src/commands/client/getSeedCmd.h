#pragma once
#include "clientCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API getSeedCmd : public clientCommand
    {
    private:
        size_t& seed;

    public:
        getSeedCmd(size_t& seed, netcode::client* client)
            : clientCommand(NC_NO_CALLBACK, client), seed(seed)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
