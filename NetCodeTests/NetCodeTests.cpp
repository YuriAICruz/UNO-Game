#include <string>
#include <iostream>

#include "client/client.h"
#include "server/server.h"

int main(int argc, char* argv[])
{
    std::cout << "running a client [c] or a server [s] ?\n";
    while (true)
    {
        std::string input;
        std::cin >> input;
        if (input == "c")
        {
            auto clientinstance = std::make_unique<client>();
            int result = clientinstance->start();
            if (result != 0)
            {
                return result;
            }
            result = clientinstance->connectToServer();
            if (result != 0)
            {
                return result;
            }

            std::string response;
            while (true)
            {
                std::cout << "\n"<<"client running, type a message to send to server. [q] to finish the application"<<"\n";
                std::cin >> input;
                if(input == "q")
                {
                    std::cout << "exiting...";
                    return clientinstance->close();
                }
                result = clientinstance->sendMessage(input, response);
                if (result != 0)
                {
                    return result;
                }
                std::cout << "Receive response: " << input << "\n";
            }
        }
        if (input == "s")
        {
            auto serverInstance = std::make_unique<server>();
            return serverInstance->run();
        }
    }

    return 0;
}
