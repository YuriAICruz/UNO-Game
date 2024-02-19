﻿#pragma once
#include <functional>
#include <future>
#include <map>

#ifndef NET_WinSock2
#define NET_WinSock2
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include "../serverCommands.h"
#include "../../framework.h"
#include "../server/room.h"

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
        room currentRoom;
        int seed = 1234;
        int id = 0;
        bool hasId = false;
        std::string lastResponse;
        struct addrinfo* addr_info;
        std::vector<room> lastRoomsList;
        std::promise<std::vector<room>>* roomsCallback;
        std::promise<room*>* roomCallback;
        std::promise<int>* seedCallback;

        std::map<std::string, std::function<void (std::string&)>> customCommands;
        std::map<std::string, std::function<void (char*, size_t)>> customRawCommands;
        std::map<std::string, std::function<void (std::string&)>> commands = {
            {
                NC_CREATE_ROOM, [this](std::string& message)
                {
                    this->createRoomCallback(message);
                }
            },
            {
                NC_LIST_ROOMS, [this](std::string& message)
                {
                    this->listRoomsCallback(message);
                }
            },
            {
                NC_GET_ROOM, [this](std::string& message)
                {
                    this->getRoomCallback(message);
                }
            },
            {
                NC_ENTER_ROOM, [this](std::string& message)
                {
                    this->enterRoomCallback(message);
                }
            },
            {
                NC_GET_SEED, [this](std::string& message)
                {
                    this->getSeedCallback(message);
                }
            },
            {
                NC_EXIT_ROOM, [this](std::string& message)
                {
                    this->exitRoomCallback(message);
                }
            },
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
        client() = default;

        int start(std::string addr = "ftp://127.0.0.1:8080");
        int connectToServer();
        void setName(const std::string& name);
        int sendMessage(const char* str);
        int close();

        room* createRoom(const std::string& roomName);
        void exitRoom();
        void enterRoom(int id);
        bool hasRoom();
        room* getRoom();
        room* getUpdatedRoom();
        int getSeed();
        std::vector<room> getRooms();
        std::string& getRoomName();
        int getRoomCount();
        int getRoomId();


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

    private:
        int initializeWinsock();
        int createSocket();
        void listenToServer();
        bool containsCommand(const std::string& string);
        bool containsCustomCommand(const std::string& command);
        bool containsCustomRawCommand(const std::string& command);

        void invalidKeyCallback(const std::string& message);
        void validKeyCallback(const std::string& message);
        void updateRoom(const std::string& message);
        void createRoomCallback(const std::string& message);
        void listRoomsCallback(const std::string& message);
        void getRoomCallback(const std::string& message);
        void getSeedCallback(const std::string& message);
        void enterRoomCallback(const std::string& message);
        void exitRoomCallback(const std::string& message);
    };
}
