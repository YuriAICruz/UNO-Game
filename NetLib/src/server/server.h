#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <atomic>
#include <WinSock2.h>
#include <map>
#include <string>
#include <unordered_set>

#include "../../framework.h"
#include "clientInfo.h"
#include "room.h"

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
    std::map<int, room> rooms;
    int connectionsCount = 0;
    int roomsCount = 0;
    int ngrokPID;
    std::atomic<bool> running{false};
    std::atomic<bool> initializing{false};
    std::atomic<bool> isListening{false};
    std::atomic<bool> error{false};

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
    bool validateKey(SOCKET clientSocket);
    std::shared_ptr<clientInfo> getClient(SOCKET uint);
    void filterCommands(std::string& message, SOCKET clientSocket);
    int createRoom(std::string roomName);
    room* getRoom(int id);
    room* getRoom(clientInfo* client);
};
