#include <string>
#include <iostream>

#include "client/client.h"
#include "server/server.h"

void inputInt(int& value)
{
    while (true)
    {
        std::string port;
        std::cout << "Insert the server port [d for 8080]\n";
        std::cin >> port;

        if (port == "d")
        {
            port = "8080";
        }

        try
        {
            value = std::stoi(port);
            break;
        }
        catch (const std::invalid_argument& e)
        {
            std::cerr << "Invalid argument: " << e.what() << std::endl;
        } catch (const std::out_of_range& e)
        {
            std::cerr << "Out of range: " << e.what() << std::endl;
        }
    }
}

int tryReconnect(std::string input, std::string addr, netcode::client* clientinstance)
{
    while (true)
    {
        std::cout << "failed to connect to server, try again [Y] [n]\n";
        std::cin >> input;
        if (input == "n")
        {
            return 1;
        }
        int result = clientinstance->start(addr);
        if (result != 0)
        {
            return result;
        }
        result = clientinstance->connectToServer();
        if (result == 0)
        {
            break;
        }
    }
    return 0;
}

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
            std::string addr;
            std::cout << "Insert the server ip or address [d for tcp://127.0.0.1:8080]\n";
            std::cin >> addr;
            if (addr == "d")
            {
                addr = "tcp://127.0.0.1:8080";
            }

            auto clientinstance = std::make_unique<netcode::client>();
            result = clientinstance->start(addr);
            if (result != 0)
            {
                return result;
            }

            result = clientinstance->connectToServer();
            if (result != 0)
            {
                result = tryReconnect(input, addr, clientinstance.get());
                if (result != 0)
                {
                    return result;
                }
            }

            while (true)
            {
                std::cout << "\nclient running, type a message to send to server. [q] to finish the application\n";
                std::cin >> input;

                if (input == "q")
                {
                    return clientinstance->close();
                }

                result = clientinstance->sendMessage(input.c_str());
                if (result != 0)
                {
                    return result;
                }
            }
        }
        if (input == "s")
        {
            int portValue;
            inputInt(portValue);
            auto serverInstance = std::make_unique<netcode::server>();
            result = serverInstance->start(portValue);
            if (result != 0)
            {
                return result;
            }
            while (!serverInstance->isRunning() && !serverInstance->hasError())
            {
            }
            if (serverInstance->hasError())
            {
                return 1;
            }
            while (serverInstance->isRunning())
            {
            }
            return serverInstance->close();
        }
    }

    return 0;
}
