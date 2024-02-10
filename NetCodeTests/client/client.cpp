#include "client.h"

#include <string>
#include <WS2tcpip.h>
#include <thread>

int client::initializeWinsock()
{
    std::cout << "initializing Winsock . . .\n";
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
    std::cout << "Winsock Initialized\n";
    return 0;
}

int client::createSocket()
{
    std::cout << "creating socket . . .\n";
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "socket creation failed\n";
        WSACleanup();
        return 1;
    }
    std::cout << "socket created\n";
    return 0;
}

int client::start(std::string addressStr)
{
    std::cout << "starting Client . . .\n";

    int r = initializeWinsock();
    if (r > 0) return r;
    r = createSocket();
    if (r > 0) return r;

    std::cout << "setting server address and port . . .\n";

    size_t pos = addressStr.find("://");
    if (pos == std::string::npos)
    {
        std::cerr << "Invalid address format\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::string addressWithoutProtocol = addressStr.substr(pos + 3);
    pos = addressWithoutProtocol.find(":");
    if (pos == std::string::npos)
    {
        std::cerr << "Invalid address format\n";
        closesocket(clientSocket);
        WSACleanup();
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
        std::cerr << "getaddrinfo failed: " << gai_strerror(addrInfoResult) << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "server address [" << host.c_str() << "] and port [" << port << "] set\n";

    return 0;
}

int client::connectToServer()
{
    std::cout << "connecting to server . . .\n";
    if (connect(clientSocket, addr_info->ai_addr, static_cast<int>(addr_info->ai_addrlen)) == SOCKET_ERROR)
    {
        std::cerr << "Connection failed\n";
        freeaddrinfo(addr_info);
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "connection successful\n";

    std::cout << "creating listen thread\n";
    std::thread clientThread([this]()
    {
        this->listenToServer();
    });
    clientThread.detach();

    int result = sendMessage(CLIENT_KEY);

    return result;
}


int client::sendMessage(const char* str) const
{
    std::cout << "sending data . . .\n";
    int sendResult = send(clientSocket, str, strlen(str), 0);
    if (sendResult == SOCKET_ERROR)
    {
        std::cerr << "Send failed\n";
        int result = close();
        return result + 1;
    }
    std::cout << "send successful\n";
    return 0;
}

int client::close() const
{
    std::cout << "closing client . . .\n";
    freeaddrinfo(addr_info);
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

void client::listenToServer()
{
    const int recvDataSize = 1024;
    char recvData[recvDataSize];
    while (true)
    {
        for (int i = 0; i < recvDataSize; ++i)
        {
            recvData[i] = ' ';
        }
        int recvSize = recv(clientSocket, recvData, strlen(recvData), 0);
        std::cout << "data received size: " << recvSize << "\n";
        if (recvSize == SOCKET_ERROR)
        {
            std::cerr << "receive failed\n";
            int result = close();
            return;
        }
        if (recvSize == 0)
        {
            std::cerr << "Connection closed by peer\n";
            int result = close();
            return;
        }

        recvData[recvSize] = '\0'; // Null-terminate received data
        lastResponse.assign(recvData, recvSize);
        std::cout << "Received: " << recvData << std::endl;
    }
}
