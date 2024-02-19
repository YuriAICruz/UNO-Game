#pragma once

#ifndef NET_WinSock2
#define NET_WinSock2
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <atomic>
#include <functional>
#include <map>
#include <string>
#include <unordered_set>

#include "../../framework.h"
#include "clientInfo.h"
#include "room.h"
#include "roomManager.h"
#include "../serverCommands.h"

namespace netcode
{
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
        std::map<size_t, std::shared_ptr<clientInfo>> clients;
        size_t connectionsCount = 0;
        int roomsCount = 0;
        int seed = 1234;
        roomManager roomManager;
        std::atomic<bool> running{false};
        std::atomic<bool> initializing{false};
        std::atomic<bool> isListening{false};
        std::atomic<bool> error{false};

        std::map<std::string, std::function<void (std::string&, SOCKET)>> customCommands;
        std::map<std::string, std::function<void (std::string&, SOCKET)>> commands = {
            {
                NC_CREATE_ROOM, [this](std::string& message, SOCKET clientConnection)
                {
                    this->createRoom(message, clientConnection);
                }
            },
            {
                NC_LIST_ROOMS, [this](std::string& message, SOCKET clientConnection)
                {
                    this->listRoom(message, clientConnection);
                }
            },
            {
                NC_ENTER_ROOM, [this](std::string& message, SOCKET clientConnection)
                {
                    this->enterRoom(message, clientConnection);
                }
            },
            {
                NC_EXIT_ROOM, [this](std::string& message, SOCKET clientConnection)
                {
                    this->exitRoom(message, clientConnection);
                }
            },
            {
                NC_GET_ROOM, [this](std::string& message, SOCKET clientConnection)
                {
                    this->getRoom(message, clientConnection);
                }
            },
            {
                NC_SET_SEED, [this](std::string& message, SOCKET clientConnection)
                {
                    this->setSeed(message, clientConnection);
                }
            },
            {
                NC_GET_SEED, [this](std::string& message, SOCKET clientConnection)
                {
                    this->getSeed(message, clientConnection);
                }
            },
            {
                NC_SET_NAME, [this](std::string& message, SOCKET clientConnection)
                {
                    this->updateClientName(message, clientConnection);
                }
            },
        };

    public:
        server() = default;
        int start(int port = 8080);
        int close();
        room* getRoom(int id);
        void broadcast(std::string msg);
        void broadcastToRoom(std::string msg, SOCKET cs);
        void broadcastToRoom(const char* responseData, size_t size, SOCKET cs);

        int getSeed() const
        {
            return seed;
        }

        void setSeed(int seed)
        {
            this->seed = seed;
        }

        bool isRunning()
        {
            return isListening || !initializing && running;
        }

        bool hasError()
        {
            return error;
        }

        void addCustomCommands(const std::map<std::string, std::function<void(std::string&, SOCKET)>>& cmds)
        {
            customCommands = cmds;
        }

    private:
        void listening();
        void disconnectClient(SOCKET clientSocket);
        void clientHandler(SOCKET clientSocket);
        bool containsCommand(const std::string& command);
        bool containsCustomCommand(const std::string& command);
        bool validateKey(SOCKET clientSocket) const;
        std::shared_ptr<clientInfo> getClient(SOCKET uint);

        void createRoom(const std::string& message, SOCKET clientSocket);
        void listRoom(const std::string& message, SOCKET clientSocket);
        void getRoom(const std::string& message, SOCKET clientSocket);
        void enterRoom(const std::string& message, SOCKET clientSocket);
        void exitRoom(const std::string& message, SOCKET clientSocket);

        void updateClientName(const std::string& message, SOCKET clientSocket);
        void getSeed(const std::string& message, SOCKET clientSocket);
        void setSeed(const std::string& message, SOCKET clientSocket);
        void sendMessage(SOCKET clientSocket, const char* responseData, int len, int flags) const;
    };
}