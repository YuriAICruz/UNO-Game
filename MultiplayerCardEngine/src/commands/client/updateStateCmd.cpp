#include "updateStateCmd.h"

namespace commands
{
    bool updateStateCmd::execute()
    {
        return true;
    }

    void updateStateCmd::callback(const std::string& message)
    {
    }

    void updateStateCmd::rawCallback(char* rawStr, int strSize)
    {
        gameManager->setStateNet(rawStr, strSize);

        if (onUpdated != nullptr)
        {
            onUpdated();
        }
    }
}
