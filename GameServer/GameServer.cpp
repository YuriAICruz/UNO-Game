#include <string>
#include <iostream>

#include "logger.h"
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

int tryReconnect(std::string input, std::string addr, client* clientinstance)
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
    logger::printCout(true);
    int portValue;
    inputInt(portValue);
    auto serverInstance = std::make_shared<server>();
    int result = serverInstance->start(portValue);
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
    logger::print((logger::printer() << "initializing ngrok for port forwarding [" << portValue << "]").str());
    std::stringstream ss;
    ss << "start cmd /c .\\ngrok.exe tcp " << portValue;
    std::string cmd = ss.str();

    auto ngrokPID = std::system(cmd.c_str());

    while (serverInstance->isRunning())
    {
    }

    if (ngrokPID > 0)
    {
        logger::print("terminating ngrok...");
        std::string killCommand = "taskkill /PID " + std::to_string(ngrokPID) + " /F";
        system(killCommand.c_str());
    }
    serverInstance->close();

    return 0;
}
