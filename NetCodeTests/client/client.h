#pragma once
#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

class client
{
private:
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;
    std::string lastResponse;

public:
    client() = default;

    int start(std::string addr = "127.0.0.1", int port = 12345);
    int connectToServer();
    int sendMessage(std::string& input);
    int close() const;
private:
    void listenToServer();
};
