#include "client.h"

#include <future>
#include <string>
#include <thread>

#include "../logger.h"
#include "../serverCommands.h"
#include "../stringUtils.h"

namespace netcode
{
    int client::initializeWinsock()
    {
        logger::print("CLIENT: initializing Winsock . . .");
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            logger::printError("CLIENT: WSAStartup failed");
            return 1;
        }
        logger::print("CLIENT: Winsock Initialized");
        return 0;
    }

    int client::createSocket()
    {
        logger::print("CLIENT: creating socket . . .");
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET)
        {
            logger::printError("CLIENT: socket creation failed");
            WSACleanup();
            return 1;
        }
        logger::print("CLIENT: socket created");
        return 0;
    }

    int client::start(std::string addressStr)
    {
        if (running)
        {
            throw std::exception("CLIENT: Client already running");
        }
        logger::print("CLIENT: starting Client . . .");

        int r = initializeWinsock();
        if (r > 0) return r;
        r = createSocket();
        if (r > 0) return r;

        logger::print("CLIENT: setting server address and port . . .");

        size_t pos = addressStr.find("://");
        if (pos == std::string::npos)
        {
            logger::printError("CLIENT: Invalid address format");
            closesocket(clientSocket);
            WSACleanup();
            error = true;
            return 1;
        }
        std::string addressWithoutProtocol = addressStr.substr(pos + 3);
        pos = addressWithoutProtocol.find(":");
        if (pos == std::string::npos)
        {
            logger::printError("CLIENT: Invalid address format");
            closesocket(clientSocket);
            WSACleanup();
            error = true;
            return 1;
        }

        std::string host = addressWithoutProtocol.substr(0, pos);
        std::string portStr = addressWithoutProtocol.substr(pos + 1);
        int port = std::stoi(portStr);

        // Resolve the server address
        addr_info = NULL;
        struct addrinfo hints;
        ZeroMemory(&hints, sizeof(hints));

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        int addrInfoResult = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &addr_info);
        if (addrInfoResult != 0)
        {
            logger::printError(
                (logger::getPrinter() << "CLIENT: getaddrinfo failed: " << gai_strerror(addrInfoResult)).str());
            closesocket(clientSocket);
            WSACleanup();
            error = true;
            return 1;
        }

        logger::print(
            (logger::getPrinter() << "CLIENT: server address [" << host.c_str() << "] and port [" <<
                std::to_string(port) <<
                "] set").str());

        running = true;

        return 0;
    }

    int client::connectToServer()
    {
        if (!running)
        {
            throw std::exception("Client not running");
        }

        logger::print("CLIENT: connecting to server . . .");

        if (connect(clientSocket, addr_info->ai_addr, static_cast<int>(addr_info->ai_addrlen)) == SOCKET_ERROR)
        {
            logger::printError("CLIENT: Connection failed");
            freeaddrinfo(addr_info);
            closesocket(clientSocket);
            WSACleanup();
            error = true;
            return 1;
        }
        logger::print("CLIENT: connection successful");

        logger::print("CLIENT: creating listen thread");
        std::thread clientThread([this]()
        {
            this->listenToServer();
        });
        clientThread.detach();

        std::promise<int> promise;
        connectingCallback = &promise;
        auto future = promise.get_future();

        int result = 0;
        if (hasId)
        {
            std::stringstream ss;
            ss << CLIENT_KEY << NC_SEPARATOR << id;
            std::string str = ss.str();
            result = sendMessage(str.c_str());
        }
        else
        {
            result = sendMessage(CLIENT_KEY);
        }

        if (result != 0)
        {
            connectingCallback = nullptr;
            return result;
        }

        future.wait();
        connectingCallback = nullptr;
        result = future.get();

        return result;
    }

    void client::setName(const std::string& name)
    {
        clientName = name;
        std::stringstream ss;
        ss << NC_SET_NAME << NC_SEPARATOR << name;
        std::string str = ss.str();
        sendMessage(str.c_str());
    }

    std::string& client::getPlayerName()
    {
        return clientName;
    }

    int client::sendMessage(const char* str)
    {
        if (!running)
        {
            logger::printError("CLIENT: Not Connected");
            error = true;
            return 1;
        }
        logger::print("CLIENT: sending data . . .");
        int sendResult = send(clientSocket, str, strlen(str), 0);
        if (sendResult == SOCKET_ERROR)
        {
            logger::printError("CLIENT: Send failed");
            int result = close();
            error = true;
            return result + 1;
        }
        logger::print("CLIENT: send successful");
        return 0;
    }

    int client::close()
    {
        if (!running)
        {
            logger::print("CLIENT: client already closed.");
            return 1;
        }

        logger::print("CLIENT: closing client . . .");

        if (running)
        {
            closesocket(clientSocket);
            WSACleanup();
        }

        if (connected)
        {
            freeaddrinfo(addr_info);
        }
        running = false;
        connected = false;
        return 0;
    }

    room* client::createRoom(const std::string& roomName)
    {
        std::promise<room*> promise;
        roomCallback = &promise;
        auto future = promise.get_future();
        std::stringstream ss;
        ss << NC_CREATE_ROOM << NC_SEPARATOR << roomName;
        sendMessage(ss.str().c_str());
        future.wait();
        roomCallback = nullptr;
        return future.get();
    }

    void client::exitRoom()
    {
        std::promise<room*> promise;
        roomCallback = &promise;
        auto future = promise.get_future();
        sendMessage(NC_EXIT_ROOM);
        future.wait();
        roomCallback = nullptr;
    }

    void client::enterRoom(int id)
    {
        std::promise<room*> promise;
        roomCallback = &promise;
        auto future = promise.get_future();
        std::stringstream ss;
        ss << NC_ENTER_ROOM << NC_SEPARATOR << id;
        sendMessage(ss.str().c_str());
        future.wait();
        roomCallback = nullptr;
    }

    bool client::hasRoom()
    {
        return currentRoom.count() > 0;
    }

    room* client::getRoom()
    {
        return &currentRoom;
    }

    room* client::getUpdatedRoom()
    {
        std::promise<room*> promise;
        roomCallback = &promise;
        std::future<room*> future = promise.get_future();

        std::stringstream ss;
        ss << NC_GET_ROOM << NC_SEPARATOR << currentRoom.getId();
        sendMessage(ss.str().c_str());

        future.wait();
        roomCallback = nullptr;
        return future.get();
    }

    int client::getSeed()
    {
        std::promise<int> promise;
        seedCallback = &promise;
        std::future<int> future = promise.get_future();

        sendMessage(NC_GET_SEED);

        future.wait();
        return future.get();
    }

    std::vector<room> client::getRooms()
    {
        std::promise<std::vector<room>> promise;
        roomsCallback = &promise;
        auto future = promise.get_future();

        sendMessage(NC_LIST_ROOMS);
        future.wait();
        roomsCallback = nullptr;
        return future.get();
    }

    std::string& client::getRoomName()
    {
        return currentRoom.getName();
    }

    int client::getRoomCount()
    {
        return currentRoom.count();
    }

    int client::getRoomId()
    {
        return currentRoom.getId();
    }

    void client::listenToServer()
    {
        isListening = true;
        const int recvDataSize = 1024;
        char recvData[recvDataSize];
        while (running)
        {
            for (int i = 0; i < recvDataSize; ++i)
            {
                recvData[i] = ' ';
            }

            int recvSize = recv(clientSocket, recvData, strlen(recvData), 0);

            logger::print((logger::getPrinter() << "CLIENT: data received size: " << recvSize).str());
            if (recvSize == SOCKET_ERROR)
            {
                logger::printError("CLIENT: receive failed");
                int result = close();
                error = true;
                break;
            }

            if (recvSize == 0)
            {
                logger::printError("CLIENT: Connection closed by peer");
                int result = close();
                error = true;
                break;
            }

            recvData[recvSize] = '\0'; // Null-terminate received data
            lastResponse.assign(recvData, recvSize);

            logger::print((logger::getPrinter() << "CLIENT: Received: " << recvData).str());
            auto data = stringUtils::splitString(lastResponse);
            if (containsCommand(data[0]))
            {
                commands[data[0]](lastResponse);
            }

            if (containsCustomCommand(data[0]))
            {
                customCommands[data[0]](lastResponse);
            }

            if (containsCustomRawCommand(data[0]))
            {
                customRawCommands[data[0]](recvData, recvSize);
            }
        }

        isListening = false;
    }

    bool client::containsCommand(const std::string& command)
    {
        auto it = commands.find(command);

        return it != commands.end();
    }

    bool client::containsCustomCommand(const std::string& command)
    {
        auto it = customCommands.find(command);

        return it != customCommands.end();
    }

    bool client::containsCustomRawCommand(const std::string& command)
    {
        auto it = customRawCommands.find(command);

        return it != customRawCommands.end();
    }

    void client::invalidKeyCallback(const std::string& message)
    {
        connected = false;
        if (connectingCallback != nullptr)
        {
            connectingCallback->set_value(1);
        }
        close();
    }

    void client::validKeyCallback(const std::string& message)
    {
        auto data = stringUtils::splitString(message);
        connected = true;
        hasId = true;
        id = std::stoul(data[1]);

        setName(clientName);

        if (connectingCallback != nullptr)
        {
            connectingCallback->set_value(0);
        }
    }

    void client::updateRoom(const std::string& message)
    {
        auto data = stringUtils::splitString(message);
        std::stringstream ss;
        for (int i = 1, n = data.size(); i < n; ++i)
        {
            ss << data[i];
            if (i < n - 1)
            {
                ss << NC_SEPARATOR;
            }
        }

        currentRoom = room::constructRoom(ss.str());

        if (onRoomUpdate != nullptr)
        {
            onRoomUpdate(&currentRoom);
        }

        if (roomCallback != nullptr)
        {
            roomCallback->set_value(&currentRoom);
        }
    }

    void client::createRoomCallback(const std::string& message)
    {
        updateRoom(message);
    }

    void client::listRoomsCallback(const std::string& message)
    {
        auto data = stringUtils::splitString(message);
        int size = std::stoi(data[1]);
        lastRoomsList.resize(size);
        int pos = 2;
        for (int i = 0; i < size; ++i)
        {
            std::stringstream ss;
            bool first = true;
            for (int n = data.size(); pos < n; ++pos)
            {
                if (!first)
                {
                    ss << NC_SEPARATOR;
                }
                if (data[pos] == NC_OBJECT_SEPARATOR)
                {
                    pos++;
                    break;
                }
                ss << data[pos];
                first = false;
            }
            lastRoomsList[i] = room::constructRoom(ss.str());
        }

        if (roomsCallback != nullptr)
        {
            roomsCallback->set_value(lastRoomsList);
        }
    }

    void client::getRoomCallback(const std::string& message)
    {
        updateRoom(message);
    }

    void client::getSeedCallback(const std::string& message)
    {
        auto data = stringUtils::splitString(message);
        seed = std::stoi(data[1]);
        if (seedCallback != nullptr)
        {
            seedCallback->set_value(seed);
            seedCallback = nullptr;
        }
    }

    void client::enterRoomCallback(const std::string& message)
    {
        updateRoom(message);
    }

    void client::exitRoomCallback(const std::string& message)
    {
        currentRoom = room();

        if (onRoomUpdate != nullptr)
        {
            onRoomUpdate(&currentRoom);
        }

        if (roomCallback != nullptr)
        {
            roomCallback->set_value(&currentRoom);
        }
    }
}
