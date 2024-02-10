#pragma once
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h> // Include for inet_pton function

#pragma comment(lib, "ws2_32.lib")

class client
{
public:
    client() = default;

    int run()
    {
        std::cout << "starting Client . . .\n";

        std::cout << "initializing Winsock . . .\n";
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cerr << "WSAStartup failed\n";
            return 1;
        }
        std::cout << "Winsock Initialized\n";

        std::cout << "creating socket . . .\n";
        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "socket creation failed\n";
            WSACleanup();
            return 1;
        }
        std::cout << "creating created\n";

        std::cout << "setting server address and port . . .\n";
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(12345);

        std::string addr = "127.0.0.1";
        if (inet_pton(AF_INET, addr.c_str(), &serverAddr.sin_addr) <= 0) {
            std::cerr << "Invalid address/ Address not supported\n";
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
        std::cout << "server address and port set\n";

        std::cout << "connecting to server . . .\n";
        if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "connection failed\n";
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
        std::cout << "connection successful\n";

        // Send data
        std::cout << "sending data "<< serverAddr.sin_addr.s_addr << ":" << serverAddr.sin_port << ". . .\n";
        const char* sendData = "Hello, server!";
        send(clientSocket, sendData, strlen(sendData), 0);

        // Receive data
        std::cout << "received data! ";
        char recvData[1024];
        int recvSize = recv(clientSocket, recvData, sizeof(recvData), 0);
        std::cout << "size: " << recvSize << "\n";
        if (recvSize > 0)
        {
            recvData[recvSize] = '\0'; // Null-terminate received data
            std::cout << "Received: " << recvData << std::endl;
        }
        
        // Close socket and cleanup
        std::cout << "closing client . . .\n";
        closesocket(clientSocket);
        WSACleanup();
    }
};
