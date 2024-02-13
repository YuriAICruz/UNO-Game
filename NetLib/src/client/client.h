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
    std::atomic<bool> running{false};
    std::atomic<bool> connected{false};
    std::atomic<bool> error{false};
    std::atomic<bool> isListening{false};
    std::string lastResponse;
    struct addrinfo* addr_info;

public:
    client() = default;

    int start(std::string addr = "ftp://127.0.0.1:8080");
    int connectToServer();
    int sendMessage(const char* str);
    int close();

    bool isRunning() const
    {
        return isListening || running;
    }

    bool isConnected() const
    {
        return running && connected && !error;
    }

    bool hasError()
    {
        return error;
    }

private:
    int initializeWinsock();
    int createSocket();
    void listenToServer();
};
