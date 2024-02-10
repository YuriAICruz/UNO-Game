#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <map>
#include <string>

#include "clientInfo.h"

class server
{
private:
    WSADATA wsaData;
    SOCKET serverSocket;
    sockaddr_in serverAddr;
    std::map<int, clientInfo> clients;
    int connectionsCount = 0;

public:
    server() = default;
    int start(int port = 12345);
    int close() const;
    void broadcast(std::string msg);

private:
    void clientHandler(SOCKET clientSocket);
};
