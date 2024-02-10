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
            std::cout << "Insert the server ip or address [d for 127.0.0.1]\n";
            std::cin >> addr;
            if (addr == "d")
            {
                addr = "127.0.0.1";
            }

            int portValue;
            inputInt(portValue);

            auto clientinstance = std::make_unique<client>();
            result = clientinstance->start(addr, portValue);
            if (result != 0)
            {
                return result;
            }
            result = clientinstance->connectToServer();
            if (result != 0)
            {
                while (true)
                {
                    std::cout << "failed to connect to server, try again [Y] [n]\n";
                    std::cin >> input;
                    if (input == "n")
                    {
                        return result;
                    }
                    result = clientinstance->start(addr, portValue);
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
            }

            while (true)
            {
                std::cout << "\n" << "client running, type a message to send to server. [q] to finish the application"
                    << "\n";
                std::cin >> input;
                if (input == "q")
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
            int portValue;
            inputInt(portValue);
            auto serverInstance = std::make_unique<server>();
            result = serverInstance->start(portValue);
            if (result != 0)
            {
                return result;
            }
            return serverInstance->close();
        }
    }

    return 0;
}
