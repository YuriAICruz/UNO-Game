﻿#include "client.h"

#include <future>
#include <string>
#include <thread>

#include "../logger.h"
#include "../serverCommands.h"
#include "../stringUtils.h"
#include "../commands/client/allReadyCmd.h"
#include "../commands/client/validateKeyCmd.h"
#include "../commands/client/setNameCmd.h"
#include "../commands/client/clientRawCommand.h"
#include "../commands/client/getUpdatedRoomCmd.h"
#include "../commands/client/setNotReadyCmd.h"
#include "../commands/client/setReadyCmd.h"
#include "../commands/client/enterRoomCmd.h"

namespace netcode
{
    client::client()
    {
        addCallback<commands::getUpdatedRoomCmd>(0);
        addCallback<commands::enterRoomCmd>(0);
        addCallback<commands::setReadyCmd>(nullptr);
        addCallback<commands::setNotReadyCmd>(nullptr);
        addCallback<commands::allReadyCmd>(nullptr);
    }

    void client::setRoom(const room& room)
    {
        currentRoom = room;

        if (onRoomUpdate != nullptr)
        {
            onRoomUpdate(&currentRoom);
        }
    }

    int client::initializeWinsock()
    {
        logger::print("CLIENT: initializing Winsock . . .");
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            logger::printError("CLIENT: WSAStartup failed");
            return 1;
        }
        logger::print("CLIENT: Winsock Initialized");
        return 0;
    }

    int client::createSocket()
    {
        logger::print("CLIENT: creating socket . . .");
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET)
        {
            logger::printError("CLIENT: socket creation failed");
            WSACleanup();
            return 1;
        }
        logger::print("CLIENT: socket created");
        return 0;
    }

    void client::callbackPendingCommands(
        const std::string& key, const std::string& message, char* rawStr, int strSize) const
    {
        for (int i = 0, n = commandsHistory.size(); i < n; ++i)
        {
            commands::clientCommand* cmd = commandsHistory[i].get();
            if (cmd->acceptRaw() && cmd->isPending(key))
            {
                commands::clientRawCommand* raw = dynamic_cast<commands::clientRawCommand*>(cmd);
                auto pos = message.find(key);
                if (pos != std::string::npos)
                {
                    rawStr += pos;
                    strSize -= pos;
                }
                raw->rawCallback(rawStr, strSize);
            }
            else if (cmd->isPending(key))
            {
                cmd->callback(message);
            }
        }
    }

    void client::callRegisteredCallbacks(
        const std::string& key, const std::string& message, char* rawStr, int strSize) const
    {
        for (int i = 0, n = callbacks.size(); i < n; ++i)
        {
            commands::clientCommand* cmd = callbacks[i].get();
            if (cmd->acceptRaw() && cmd->hasKey(key))
            {
                commands::clientRawCommand* raw = dynamic_cast<commands::clientRawCommand*>(cmd);
                char* newPos = stringUtils::findWordStart(rawStr, strSize, key.c_str());
                if (newPos != nullptr)
                {
                    rawStr = newPos;
                }
                raw->rawCallback(rawStr, strSize);
            }
            else if (cmd->hasKey(key))
            {
                cmd->callback(message);
            }
        }
    }

    int client::start(std::string addressStr)
    {
        if (running)
        {
            throw std::exception("CLIENT: Client already running");
        }
        logger::print("CLIENT: starting Client . . .");

        int r = initializeWinsock();
        if (r > 0) return r;
        r = createSocket();
        if (r > 0) return r;

        logger::print("CLIENT: setting server address and port . . .");

        size_t pos = addressStr.find("://");
        if (pos == std::string::npos)
        {
            logger::printError("CLIENT: Invalid address format");
            closesocket(clientSocket);
            WSACleanup();
            error = true;
            return 1;
        }
        std::string addressWithoutProtocol = addressStr.substr(pos + 3);
        pos = addressWithoutProtocol.find(":");
        if (pos == std::string::npos)
        {
            logger::printError("CLIENT: Invalid address format");
            closesocket(clientSocket);
            WSACleanup();
            error = true;
            return 1;
        }

        std::string host = addressWithoutProtocol.substr(0, pos);
        std::string portStr = addressWithoutProtocol.substr(pos + 1);
        int port = std::stoi(portStr);

        // Resolve the server address
        addr_info = NULL;
        struct addrinfo hints;
        ZeroMemory(&hints, sizeof(hints));

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        int addrInfoResult = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &addr_info);
        if (addrInfoResult != 0)
        {
            logger::printError(
                (logger::getPrinter() << "CLIENT: getaddrinfo failed: " << gai_strerror(addrInfoResult)).str());
            closesocket(clientSocket);
            WSACleanup();
            error = true;
            return 1;
        }

        logger::print(
            (logger::getPrinter() << "CLIENT: server address [" << host.c_str() << "] and port [" <<
                std::to_string(port) <<
                "] set").str());

        running = true;

        return 0;
    }

    int client::connectToServer()
    {
        if (!running)
        {
            throw std::exception("Client not running");
        }

        logger::print("CLIENT: connecting to server . . .");

        if (connect(clientSocket, addr_info->ai_addr, static_cast<int>(addr_info->ai_addrlen)) == SOCKET_ERROR)
        {
            logger::printError("CLIENT: Connection failed");
            freeaddrinfo(addr_info);
            closesocket(clientSocket);
            WSACleanup();
            error = true;
            return 1;
        }
        logger::print("CLIENT: connection successful");

        logger::print("CLIENT: creating listen thread");
        std::thread clientThread([this]()
        {
            this->listenToServer();
        });
        clientThread.detach();

        std::string key = CLIENT_KEY;
        bool success = executeCommand<commands::validateKeyCmd>(key, id, hasId);
        executeCommand<commands::setNameCmd>(clientName);

        return success ? 0 : 1;
    }

    std::string& client::getPlayerName()
    {
        return clientName;
    }

    void client::roomReady(bool ready) const
    {
        if(onRoomReady != nullptr)
        {
            onRoomReady(ready);
        }
    }

    int client::sendMessage(std::string str)
    {
        return sendMessage(str.c_str());
    }

    int client::sendMessage(const char* str)
    {
        if (!running)
        {
            logger::printError("CLIENT: Not Connected");
            error = true;
            return 1;
        }

        logger::print((logger::getPrinter() << "CLIENT: sending data size [" << strlen(str) << "]").str());

        std::stringstream ss;
        ss << str << NC_COMMAND_END;

        std::string newStr = ss.str();
        const char* msg = newStr.c_str();
        int sendResult = send(clientSocket, msg, strlen(msg), 0);
        if (sendResult == SOCKET_ERROR)
        {
            logger::printError("CLIENT: Send failed");
            int result = close();
            error = true;
            return result + 1;
        }
        logger::print("CLIENT: send successful");
        return 0;
    }

    int client::close()
    {
        if (!running)
        {
            logger::print("CLIENT: client already closed.");
            return 1;
        }

        logger::print("CLIENT: closing client . . .");

        if (running)
        {
            closesocket(clientSocket);
            WSACleanup();
        }

        if (connected && running)
        {
            freeaddrinfo(addr_info);
        }
        running = false;
        connected = false;
        return 0;
    }

    void client::listenToServer()
    {
        isListening = true;
        const int recvDataSize = NC_PACKET_SIZE;
        char recvData[recvDataSize];
        while (running)
        {
            for (int i = 0; i < recvDataSize; ++i)
            {
                recvData[i] = ' ';
            }

            logger::print("CLIENT: listening . . .");
            int recvSize = recv(clientSocket, recvData, strlen(recvData), 0);

            logger::print((logger::getPrinter() << "CLIENT: data received size: " << recvSize).str());
            if (recvSize == SOCKET_ERROR)
            {
                logger::printError("CLIENT: receive failed");
                int result = close();
                error = true;
                break;
            }

            if (recvSize == 0)
            {
                logger::printError("CLIENT: Connection closed by peer");
                int result = close();
                error = true;
                break;
            }

            if (recvSize < recvDataSize)
            {
                recvData[recvSize] = '\0'; // Null-terminate received data
            }
            recvSize = min(recvDataSize-1, recvSize);
            lastResponse.assign(recvData, recvSize);

            logger::print((logger::getPrinter() << "CLIENT: Received: " << recvData).str());
            auto commandsBuffer = stringUtils::splitString(lastResponse, NC_COMMAND_END);
            for (auto& command : commandsBuffer)
            {
                auto data = stringUtils::splitString(command);
                callbackPendingCommands(data[0], command, recvData, recvSize);
                callRegisteredCallbacks(data[0], command, recvData, recvSize);
            }
        }

        isListening = false;
    }
}
