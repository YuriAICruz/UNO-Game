#pragma once
#include <iostream>
#include <WinSock2.h>
#include "../../framework.h"

#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#define CLIENT_KEY "server_debug"
#else
#define CLIENT_KEY "s_p_56489135"
#endif //_DEBUG

class NETCODE_API client
{
private:
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;
    bool running = false;
    std::string lastResponse;
    struct addrinfo* addr_info;

public:
    client() = default;

    int initializeWinsock();
    int createSocket();
    int start(std::string addr = "ftp://127.0.0.1:8080");
    int connectToServer();
    int sendMessage(const char* str);
    int close();

    bool isRunning()
    {
        return running;
    }

private:
    void listenToServer();
};
