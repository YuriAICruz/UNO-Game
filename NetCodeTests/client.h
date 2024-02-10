#pragma once
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h> // Include for inet_pton function

#pragma comment(lib, "ws2_32.lib")

class client
{
public:
    client() = default;

    int start(std::string addr = "127.0.0.1", int port = 12345);
    int connectToServer();
    int sendMessage(std::string& input, std::string& response);
    int close() const;
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;
};
