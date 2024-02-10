#include <string>
#include <iostream>

#include "client/client.h"
#include "server/server.h"

int main(int argc, char* argv[])
{
    std::cout << "running a client [c] or a server [s] ?\n";
    while (true)
    {
        int result = 0;
        std::string input;
        std::cin >> input;
        if (input == "c")
        {
            auto clientinstance = std::make_unique<client>();
            result = clientinstance->start();
            if (result != 0)
            {
                return result;
            }
            result = clientinstance->connectToServer();
            if (result != 0)
            {
                while (true)
                {
                    std::cout <<"failed to connect to server, try again [Y] [n]\n";
                    std::cin >> input;
                    if(input == "n")
                    {
                        return result;
                    }   
                    result = clientinstance->connectToServer();
                    if(result == 0)
                    {
                        break;
                    }
                }
            }

            while (true)
            {
                std::cout << "\n"<<"client running, type a message to send to server. [q] to finish the application"<<"\n";
                std::cin >> input;
                if(input == "q")
                {
                    std::cout << "exiting...";
                    return clientinstance->close();
                }
                result = clientinstance->sendMessage(input);
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
            result = serverInstance->start();
            if(result != 0)
            {
                return result;
            }
            return serverInstance->close();
        }
    }

    return 0;
}
