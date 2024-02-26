#pragma once

#include <atomic>
#include <functional>
#include <map>
#include <string>
#include <unordered_set>

#include "clientInfo.h"
#include "room.h"
#include "roomManager.h"
#include "../../framework.h"
#include "../winSockImp.h"
#include "../serverCommands.h"
#include "../commands/server/serverCommand.h"
#include "../commands/server/enterRoomServerCmd.h"


namespace netcode
{
    class NETCODE_API server
    {
    private:
#ifdef _DEBUG
        const std::unordered_set<std::string> validKeys = {"server_debug"};
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

        std::vector<std::unique_ptr<commands::serverCommand>> commandsHistory;
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
            {
                NC_ROOM_READY_STATUS, [this](std::string& message, SOCKET clientConnection)
                {
                    this->updateRoomStatus(message, clientConnection);
                }
            },
        };

    public:
        std::function<void(clientInfo*)> onClientReconnected;
        std::function<void (room* room)> onRoomCreated;

        server();

        int start(int port = 8080);
        int close();
        room* getRoom(int id);
        void broadcastUpdatedRoom(SOCKET clientSocket);
        void sendRoomData(SOCKET clientSocket, int id);
        bool broadcast(std::string msg) const;
        bool broadcastToRoom(std::string msg, SOCKET cs);
        bool broadcastToRoomRaw(const char* responseData, size_t size, SOCKET cs);
        bool sendMessage(std::string message, SOCKET clientSocket) const;
        bool sendMessage(const char* message, SOCKET clientSocket) const;
        bool sendMessageRaw(SOCKET clientSocket, const char* responseData, int len, int flags) const;

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

        template <typename T, typename... Args>
        bool NETCODE_API executeServerCommand(Args&&... args)
        {
            commandsHistory.push_back(std::make_unique<T>(std::forward<Args>(args)..., &roomManager, this));
            return commandsHistory.back()->execute();
        }

        void lockRoom(SOCKET cs);
        void unlockRoom(SOCKET cs);
        bool isRoomReady(int roomId);

        void listening();
        void disconnectClient(SOCKET clientSocket);
        void clientReconnected(const std::shared_ptr<clientInfo>& client, SOCKET uint);
        void callbackPendingCommands(const std::string& key, std::vector<std::string>& data, SOCKET clientSocket) const;
        void clientHandler(SOCKET clientSocket);
        bool containsCommand(const std::string& command);
        bool containsCustomCommand(const std::string& command);
        bool validateKey(SOCKET clientSocket, int& id) const;
        std::shared_ptr<clientInfo> getClient(SOCKET uint);
        std::shared_ptr<clientInfo> getClientFromId(size_t id) const;

        void createRoom(const std::string& message, SOCKET clientSocket);
        void listRoom(const std::string& message, SOCKET clientSocket);
        void getRoom(const std::string& message, SOCKET clientSocket);
        void exitRoom(const std::string& message, SOCKET clientSocket);
        void updateRoomStatus(const std::string& message, SOCKET clientSocket);

        void updateClientName(const std::string& message, SOCKET clientSocket);
        void getSeed(const std::string& message, SOCKET clientSocket);
        void setSeed(const std::string& message, SOCKET clientSocket);

    private:
    };
}
