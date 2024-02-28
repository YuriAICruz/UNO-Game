#pragma once
#include <functional>
#include <future>
#include <map>

#include "../winSockImp.h"
#include "../serverCommands.h"
#include "../../framework.h"
#include "../server/room.h"
#include "../commands/client/clientCommand.h"

#ifdef _DEBUG
#define CLIENT_KEY "server_debug"
#else
#define CLIENT_KEY "s_p_56489135"
#endif //_DEBUG

namespace commands
{
    class callback;
    class clientRawCommand;
    class createRoomCmd;
    class enterRoomCmd;
    class exitRoomCmd;
    class hasRoomCmd;
    class getUpdatedRoomCmd;
    class getSeedCmd;
    class getRoomCmd;
    class setNameCmd;
    class validateKeyCmd;
}

namespace netcode
{
    class NETCODE_API client
    {
    private:
        WSADATA wsaData;
        SOCKET clientSocket = 0;
        sockaddr_in serverAddr;

        std::atomic<bool> running{false};
        std::atomic<bool> connected{false};
        std::atomic<bool> error{false};
        std::atomic<bool> isListening{false};
        friend class commands::validateKeyCmd;

        std::string clientName = "Player";
        friend class commands::setNameCmd;
        room currentRoom;
        friend class commands::getRoomCmd;
        int seed = 1234;
        friend class commands::getSeedCmd;

        uint16_t id = 0;
        bool hasId = false;
        std::string lastResponse = "";
        struct addrinfo* addr_info = nullptr;

        std::vector<std::unique_ptr<commands::clientCommand>> commandsHistory;
        std::vector<std::unique_ptr<commands::clientCommand>> callbacks;

    public:
        std::function<void (room*)> onRoomUpdate;
        std::function<void (bool)> onRoomReady;
        client();

        int start(std::string addr = "ftp://127.0.0.1:8080");
        int connectToServer();
        std::string& getPlayerName();
        void roomReady(bool ready) const;
        int sendMessage(std::string str);
        int sendMessage(const char* str);
        int close();


        uint16_t getId() const
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

        template <typename T, typename... Args>
        bool NETCODE_API executeCommand(Args&&... args)
        {
            commandsHistory.push_back(std::make_unique<T>(std::forward<Args>(args)..., this));
            return commandsHistory.back()->execute();
        }

        template <typename T, typename... Args>
        void NETCODE_API addCallback(Args&&... args)
        {
            callbacks.push_back(std::make_unique<T>(std::forward<Args>(args)..., this));
            callbacks.back()->setAsCallbackOnly();
        }

    private:
        void setRoom(const room& room);
        friend class commands::createRoomCmd;
        friend class commands::enterRoomCmd;
        friend class commands::exitRoomCmd;
        friend class commands::getUpdatedRoomCmd;
        friend class commands::hasRoomCmd;

        int initializeWinsock();
        int createSocket();
        void callbackPendingCommands(const std::string& key, const std::string& message, char* rawStr,
                                     int strSize) const;
        void callRegisteredCallbacks(const std::string& key, const std::string& message, char* rawStr,
                                     int strSize) const;
        void listenToServer();
    };
}
