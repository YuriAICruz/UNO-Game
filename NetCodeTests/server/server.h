#pragma once
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h> // Include for inet_pton and inet_ntop functions

#pragma comment(lib, "ws2_32.lib")

class server
{
public:
    server() = default;

    int run()
    {
        std::cout << "starting server . . .\n";
        std::cout << "initializing Winsock . . .\n";
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cerr << "WSAStartup failed\n";
            return 1;
        }
        std::cout << "Winsock initialized\n";

        std::cout << "creating socket . . .\n";
        SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET)
        {
            std::cerr << "Socket creation failed\n";
            WSACleanup();
            return 1;
        }
        std::cout << "socket created\n";

        std::cout << "binding socket . . .\n";
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(12345); // Example port

        if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
        {
            std::cerr << "Bind failed\n";
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }
        std::cout << "bind success to port: " << serverAddr.sin_port << "\n";

        std::cout << "setup listening . . .\n";
        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
        {
            std::cerr << "Listen failed\n";
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Server listening on port " << serverAddr.sin_port << "...\n";

        SOCKET clientSocket;
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        char clientIP[INET_ADDRSTRLEN];

        while (true)
        {
            std::cout << "waiting for connections . . .\n";
            clientSocket = accept(serverSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrSize);
            if (clientSocket == INVALID_SOCKET)
            {
                std::cerr << "Accept failed\n";
                closesocket(serverSocket);
                WSACleanup();
                return 1;
            }

            inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
            std::cout << "Connection accepted from " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;

            std::cout << "waiting for client data . . .\n";
            char recvData[1024];
            int recvSize = recv(clientSocket, recvData, sizeof(recvData), 0);
            if (recvSize > 0)
            {
                recvData[recvSize] = '\0'; // Null-terminate received data
                std::cout << "Received: " << recvData << std::endl;


                std::cout << "sending automatic response\n";
                const char* responseData = "Message received by server!";
                send(clientSocket, responseData, strlen(responseData), 0);
            }

            std::cout << "closing client connection [" << clientSocket << "]\n";
            closesocket(clientSocket);
        }

        std::cout << "shutdown server . . .\n";
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }
};
