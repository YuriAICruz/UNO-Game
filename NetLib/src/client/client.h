﻿#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <WinSock2.h>

#include "../serverCommands.h"
#include "../../framework.h"
#include "../server/room.h"

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
    room currentRoom;
    std::string lastResponse;
    struct addrinfo* addr_info;
    std::function<void(std::vector<room>)> roomsCallback;
    std::vector<room> lastRoomsList;

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
            NC_ENTER_ROOM, [this](std::string& message)
            {
                this->enterRoomCallback(message);
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
    client() = default;

    int start(std::string addr = "ftp://127.0.0.1:8080");
    int connectToServer();
    int sendMessage(const char* str);
    int close();

    void createRoom(const std::string& roomName);
    void exitRoom();
    void enterRoom(int id);
    bool hasRoom();
    void getRooms(std::function<void (std::vector<room>)> callback);
    std::string& getRoomName();
    int getRoomId();


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
    bool containsCommand(const std::string& string);

    void invalidKeyCallback(const std::string& message);
    void validKeyCallback(const std::string& message);
    void createRoomCallback(const std::string& message);
    void listRoomsCallback(const std::string& message);
    void enterRoomCallback(const std::string& message);
    void exitRoomCallback(const std::string& message);
};