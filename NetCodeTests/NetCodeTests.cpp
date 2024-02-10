#include <string>
#include <iostream>

#include "client.h"
#include "server.h"
int main(int argc, char* argv[])
{
    std::cout << "runing a client [c] or a server [s]\n";
    while (true)
    {
        std::string input;
        std::cin >> input;
        if (input == "c")
        {
            auto clientinstance = std::make_unique<client>();
            return clientinstance->run();
        }
        if (input == "s")
        {
            auto serverInstance = std::make_unique<server>();
            return serverInstance->run();
        }
    }

    return 0;
}
