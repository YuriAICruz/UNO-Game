#include "client.h"
#include <WS2tcpip.h>

int client::start(std::string addr, int port)
{
    std::cout << "starting Client . . .\n";

    std::cout << "initializing Winsock . . .\n";
    wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
    std::cout << "Winsock Initialized\n";

    std::cout << "creating socket . . .\n";
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "socket creation failed\n";
        WSACleanup();
        return 1;
    }
    std::cout << "creating created\n";

    std::cout << "setting server address and port . . .\n";
    serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, addr.c_str(), &serverAddr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "server address [" << addr << "] and port [" << port << "] set\n";

    return 0;
}

int client::connectToServer()
{
    std::cout << "connecting to server . . .\n";
    if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "connection failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "connection successful\n";

    return 0;
}

int client::sendMessage(std::string& input, std::string& response)
{
    std::cout << "sending data . . .\n";
    auto data = input.c_str();
    int sendResult = send(clientSocket, data, strlen(data), 0);
    if (sendResult == SOCKET_ERROR) {
        std::cerr << "Send failed\n";
        int result = close();
        return result + 1;
    }
    std::cout << "send successful\n";

    std::cout << "received data! ";
    char recvData[1024];
    int recvSize = recv(clientSocket, recvData, strlen(recvData), 0);
    std::cout << "size: " << recvSize << "\n";
    if (recvSize == SOCKET_ERROR)
    {
        std::cerr << "receive failed\n";
        int result = close();
        return result + 1;
    }
    if (recvSize == 0)
    {
        std::cerr << "Connection closed by peer\n";
        int result = close();
        return result + 1;
    }

    recvData[recvSize] = '\0'; // Null-terminate received data
    response.assign(recvData, recvSize);
    std::cout << "Received: " << recvData << std::endl;
    return 0;
}

int client::close() const
{
    std::cout << "closing client . . .\n";
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
