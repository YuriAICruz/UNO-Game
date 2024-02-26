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
        int seed = 1234;
        roomManager roomManager;
        std::atomic<bool> running{false};
        std::atomic<bool> initializing{false};
        std::atomic<bool> isListening{false};
        std::atomic<bool> error{false};

        std::vector<std::unique_ptr<commands::serverCommand>> commandsHistory;
        std::map<std::string, std::function<void (std::string&, SOCKET)>> customCommands;

    public:
        std::function<void(clientInfo*)> onClientReconnected;
        std::function<void (room* room)> onRoomCreated;

        server();

        int start(int port = 8080);
        int close();
        void broadcastUpdatedRoom(SOCKET clientSocket);
        void sendRoomData(SOCKET clientSocket, int id);
        std::shared_ptr<clientInfo> getClient(SOCKET uint);
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

        void setSeed(size_t seed)
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

    private:
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
        std::shared_ptr<clientInfo> getClientFromId(size_t id) const;

        void getRoom(const std::string& message, SOCKET clientSocket);
    };
}
