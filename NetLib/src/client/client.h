#pragma once
#include <functional>
#include <future>
#include <map>

#include "../winSockImp.h"
#include "../serverCommands.h"
#include "../../framework.h"
#include "../server/room.h"
#include "../commands/client/clientCommand.h"
#include "../commands/client/createRoomCmd.h"
#include "../commands/client/enterRoomCmd.h"
#include "../commands/client/exitRoomCmd.h"
#include "../commands/client/getRoomCmd.h"
#include "../commands/client/getSeedCmd.h"
#include "../commands/client/getUpdatedRoomCmd.h"

#ifdef _DEBUG
#define CLIENT_KEY "server_debug"
#else
#define CLIENT_KEY "s_p_56489135"
#endif //_DEBUG

namespace netcode
{
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
        std::string clientName = "Player";
        room currentRoom;
        friend class commands::getRoomCmd;
        int seed = 1234;
        friend class commands::getSeedCmd;
        int id = 0;
        bool hasId = false;
        std::string lastResponse;
        struct addrinfo* addr_info;
        std::promise<int>* connectingCallback;
        std::promise<std::vector<room>>* roomsCallback;
        std::promise<room*>* roomCallback;
        std::promise<bool>* roomReadyCallback;
        std::promise<int>* seedCallback;

        std::vector<std::unique_ptr<commands::clientCommand>> commandsHistory;
        std::map<std::string, std::function<void (std::string&)>> customCommands;
        std::map<std::string, std::function<void (char*, size_t)>> customRawCommands;
        std::map<std::string, std::function<void (std::string&)>> commands = std::map<
            std::string, std::function<void (std::string&)>>{
            {
                NC_VALID_KEY, [this](std::string& message)
                {
                    this->validKeyCallback(message);
                }
            },
            {
                NC_INVALID_KEY, [this](std::string& message)
                {
                    this->invalidKeyCallback(message);
                }
            },
        };

    public:
        std::function<void (room*)> onRoomUpdate;
        std::function<void (bool)> onRoomReady;
        client() = default;

        int start(std::string addr = "ftp://127.0.0.1:8080");
        int connectToServer();
        void setName(const std::string& name);
        std::string& getPlayerName();
        int sendMessage(std::string str);
        int sendMessage(const char* str);
        int close();

        int getId() const
        {
            return id;
        }

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

        void addCustomCommands(const std::map<std::string, std::function<void(std::string&)>>& cmds)
        {
            customCommands = cmds;
        }

        void addCustomRawCommands(const std::map<std::string, std::function<void(char*, size_t)>>& cmds)
        {
            customRawCommands = cmds;
        }

        template <typename T, typename... Args>
        bool NETCODE_API executeCommand(Args&&... args)
        {
            commandsHistory.push_back(std::make_unique<T>(std::forward<Args>(args)..., this));
            return commandsHistory.back()->execute();
        }

    private:
        void setRoom(const room& room);
        friend class commands::createRoomCmd;
        friend class commands::enterRoomCmd;
        friend class commands::exitRoomCmd;
        friend class commands::getUpdatedRoomCmd;

        int initializeWinsock();
        int createSocket();
        void callbackPendingCommands(const std::string& key, const std::string& message) const;
        void listenToServer();
        bool containsCommand(const std::string& string);
        bool containsCustomCommand(const std::string& command);
        bool containsCustomRawCommand(const std::string& command);

        void invalidKeyCallback(const std::string& message);
        void validKeyCallback(const std::string& message);
    };
}
