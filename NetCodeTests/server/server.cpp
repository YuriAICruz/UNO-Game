#include "server.h"
#include <iostream>
#include <WS2tcpip.h>
#include <thread>

int server::start(int port)
{
    std::cout << "starting server . . .\n";
    std::cout << "initializing Winsock . . .\n";
    wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
    std::cout << "Winsock initialized\n";

    std::cout << "creating socket . . .\n";
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }
    std::cout << "socket created\n";

    std::cout << "binding socket . . .\n";
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "bind success to port: " << port << "\n";

    std::cout << "setup listening . . .\n";
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port " << serverAddr.sin_port << "...\n";

    char clientIP[INET_ADDRSTRLEN];

    while (true)
    {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        std::cout << "waiting for connections . . .\n";
        SOCKET currentClientSocket = accept(serverSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrSize);
        if (currentClientSocket == INVALID_SOCKET)
        {
            std::cerr << "Accept failed\n";
            int result = close();
            return result + 1;
        }

        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        std::cout << "Connection accepted from " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;

        std::thread clientThread([this, currentClientSocket]()
        {
            this->clientHandler(currentClientSocket);
        });
        clientThread.detach();
    }
}

void server::broadcast(std::string msg)
{
    for (auto client : clients)
    {
        auto responseData = msg.c_str();
        send(*client.second.connection, responseData, strlen(responseData), 0);
    }
}

int server::close() const
{
    std::cout << "shutdown server . . .\n";
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

void server::clientHandler(SOCKET clientSocket)
{
    std::cout << "waiting for client data . . .\n";

    clientInfo client = clientInfo();
    client.connection = &clientSocket;
    clients.insert(std::make_pair(connectionsCount, client));
    connectionsCount++;

    char recvData[1024];
    int recvSize;

    while ((recvSize = recv(clientSocket, recvData, sizeof(recvData), 0)) > 0)
    {
        recvData[recvSize] = '\0';
        std::cout << "Received from client: " << recvData << std::endl;

        std::cout << "sending automatic response\n";
        const char* responseData = "Message received by server!";        
        send(clientSocket, responseData, strlen(responseData), 0);
        broadcast("This message is for all!!");
        // Echo received data back to client
        //send(clientSocket, recvData, recvSize, 0);
    }

    if (recvSize == 0)
    {
        std::cout << "Client disconnected.\n";
    }
    else
    {
        std::cerr << "Receive failed\n";
    }

    std::cout << "closing client connection [" << clientSocket << "]\n";
    closesocket(clientSocket);
    clients.erase(connectionsCount);
}
