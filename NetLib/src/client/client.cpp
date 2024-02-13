#include "client.h"

#include <string>
#include <WS2tcpip.h>
#include <thread>
#include "../logger.h"
#include "../serverCommands.h"

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
        (logger::getPrinter() << "CLIENT: server address [" << host.c_str() << "] and port [" << std::to_string(port) <<
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

    int result = sendMessage(CLIENT_KEY);

    return result;
}


int client::sendMessage(const char* str)
{
    logger::print("CLIENT: sending data . . .");
    int sendResult = send(clientSocket, str, strlen(str), 0);
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
    freeaddrinfo(addr_info);
    closesocket(clientSocket);
    WSACleanup();

    running = false;
    connected = false;
    return 0;
}

void client::listenToServer()
{
    isListening = true;
    const int recvDataSize = 1024;
    char recvData[recvDataSize];
    while (running)
    {
        for (int i = 0; i < recvDataSize; ++i)
        {
            recvData[i] = ' ';
        }

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

        recvData[recvSize] = '\0'; // Null-terminate received data
        lastResponse.assign(recvData, recvSize);

        logger::print((logger::getPrinter() << "CLIENT: Received: " << recvData).str());

        if (lastResponse == NC_VALID_KEY)
        {
            connected = true;
        }
        else if (lastResponse == NC_INVALID_KEY)
        {
            connected = false;
            close();
        }
    }

    isListening = false;
}
