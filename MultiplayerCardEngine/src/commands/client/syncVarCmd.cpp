#include "syncVarCmd.h"

#include "stringUtils.h"

namespace commands
{
    bool syncVarCmd::execute()
    {
        std::promise<bool> promise;
        auto future = setPromise(promise);

        gameManager->updateVarsDictionary(id, value);

        std::stringstream ss;
        ss << cmdKey << NC_SEPARATOR << id << NC_SEPARATOR << value;
        std::string str = ss.str();

        netClient->sendMessage(str.c_str());

        return waitAndReturnPromise(future);
    }

    void syncVarCmd::callback(const std::string& message)
    {
        auto data = stringUtils::splitString(message);

        const int id = stoi(data[1]);
        const int value = stoi(data[2]);

        gameManager->updateVarsDictionary(id, value);

        setCallback(true);
    }
}
