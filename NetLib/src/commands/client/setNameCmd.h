#pragma once
#include "clientCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API setNameCmd : public clientCommand
    {
    private:
        std::string name;

    public:
        setNameCmd(const std::string& clientName, netcode::client* client)
            : clientCommand(NC_SET_NAME, client), name(clientName)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
    };
}
