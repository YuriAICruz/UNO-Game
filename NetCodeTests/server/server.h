#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <map>
#include <string>
#include <unordered_set>

#include "clientInfo.h"
#include "room.h"

class server
{
private:
    const std::unordered_set<std::string> validKeys = {"s_p_56489135", "server_debug"};
    WSADATA wsaData;
    SOCKET serverSocket;
    sockaddr_in serverAddr;
    std::map<int, clientInfo> clients;
    std::map<int, room> rooms;
    int connectionsCount = 0;
    int roomsCount = 0;
    int ngrokPID;

public:
    server() = default;
    int start(int port = 12345);
    int close() const;
    void broadcast(std::string msg);

private:
    void clientHandler(SOCKET clientSocket);
    bool validateKey(SOCKET clientSocket);
    clientInfo* getClient(SOCKET uint);
    void filterCommands(std::string& message, SOCKET clientSocket);
    int createRoom();
    room* getRoom(int id);
    static std::vector<std::string> splitString(const std::string& s);
};
