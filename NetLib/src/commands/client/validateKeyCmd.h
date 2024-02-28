#pragma once
#include "clientCommand.h"
#include "../../serverCommands.h"

namespace commands
{
    class NETCODE_API validateKeyCmd : public clientCommand
    {
    private:
        std::string authKey;
        bool hasId;
        uint16_t id = 0;

    public:
        validateKeyCmd(std::string& authKey, uint16_t id, bool hasId, netcode::client* client)
            : clientCommand(NC_VALID_KEY, client), authKey(authKey), id(id), hasId(hasId)
        {
        }

        bool execute() override;
        void callback(const std::string& message) override;
        bool isPending(const std::string& key) const override;
    };
}
