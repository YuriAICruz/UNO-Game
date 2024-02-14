#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <atomic>
#include <functional>
#include <WinSock2.h>
#include <map>
#include <string>
#include <unordered_set>

#include "../../framework.h"
#include "clientInfo.h"
#include "room.h"
#include "roomManager.h"
#include "../serverCommands.h"

class NETCODE_API server
{
private:
#ifdef _DEBUG
    const std::unordered_set<std::string> validKeys = {"s_p_56489135", "server_debug"};
#else
    const std::unordered_set<std::string> validKeys = {"s_p_56489135"};
#endif //_DEBUG
    WSADATA wsaData;
    SOCKET serverSocket;
    sockaddr_in serverAddr;
    std::map<int, std::shared_ptr<clientInfo>> clients;
    int connectionsCount = 0;
    int roomsCount = 0;
    int ngrokPID;
    roomManager roomManager;
    std::atomic<bool> running{false};
    std::atomic<bool> initializing{false};
    std::atomic<bool> isListening{false};
    std::atomic<bool> error{false};

    std::map<std::string, std::function<void (std::string&, SOCKET)>> commands = {
        {NC_CREATE_ROOM, [this](std::string& message, SOCKET clientConnection)
        {
            this->createRoom(message, clientConnection);
        }},
        {NC_LIST_ROOMS, [this](std::string& message, SOCKET clientConnection)
        {
            this->listRoom(message, clientConnection);
        }},
        {NC_ENTER_ROOM, [this](std::string& message, SOCKET clientConnection)
        {
            this->enterRoom(message, clientConnection);
        }},
        {NC_EXIT_ROOM, [this](std::string& message, SOCKET clientConnection)
        {
            this->exitRoom(message, clientConnection);
        }},
    };

public:
    server() = default;
    int start(int port = 8080);
    int close();
    void broadcast(std::string msg);

    bool isRunning()
    {
        return isListening || !initializing && running;
    }

    bool hasError()
    {
        return error;
    }

private:
    void listening();
    void clientHandler(SOCKET clientSocket);
    bool validateKey(SOCKET clientSocket) const;
    std::shared_ptr<clientInfo> getClient(SOCKET uint);

    void createRoom(const std::string& message, SOCKET clientSocket);
    void listRoom(const std::string& message, SOCKET clientSocket);
    void enterRoom(const std::string& message, SOCKET clientSocket);
    void exitRoom(const std::string& message, SOCKET clientSocket);
};
