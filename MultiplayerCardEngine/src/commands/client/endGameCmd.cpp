#include "endGameCmd.h"

namespace commands
{
    bool endGameCmd::execute()
    {
        return true;
    }

    void endGameCmd::callback(const std::string& message)
    {
        gameManager->endGame();
    }
}
