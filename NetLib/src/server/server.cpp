#include "server.h"
#include <WS2tcpip.h>
#include <thread>
#include <sstream>

#include "../logger.h"
#include "../serverCommands.h"
#include "../stringUtils.h"

namespace netcode
{
    int server::start(int port)
    {
        error = false;
        logger::print("SERVER: starting server . . .");
        logger::print("SERVER: initializing Winsock . . .");
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            logger::printError("WSAStartup failed");
            error = true;
            return 1;
        }
        logger::print("SERVER: Winsock initialized");

        logger::print("SERVER: creating socket . . .");
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET)
        {
            logger::printError("SERVER: Socket creation failed");
            WSACleanup();
            error = true;
            return 1;
        }
        logger::print("SERVER: socket created");

        logger::print("SERVER: binding socket . . .");
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
        {
            logger::printError("SERVER: Bind failed");
            closesocket(serverSocket);
            WSACleanup();
            error = true;
            return 1;
        }
        logger::print((logger::getPrinter() << "SERVER: bind success to port: " << port << "").str());

        logger::print("setup listening . . .");
        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
        {
            logger::printError("SERVER: Listen failed");
            closesocket(serverSocket);
            WSACleanup();
            error = true;
            return 1;
        }

        logger::print((logger::getPrinter() << "SERVER: Server listening on port " << port << "...").str());

        initializing = true;
        running = true;

        std::thread serverListeningThread([this]()
        {
            this->listening();
        });
        serverListeningThread.detach();

        return 0;
    }

    void server::broadcast(std::string msg)
    {
        for (auto pair : clients)
        {
            if (pair.second->isConnected)
            {
                auto responseData = msg.c_str();
                sendMessage(*pair.second->connection, responseData, strlen(responseData), 0);
            }
        }
    }

    void server::broadcastToRoom(std::string msg, SOCKET cs)
    {
        auto room = roomManager.getRoom(getClient(cs).get());
        for (clientInfo* pair : room->clients())
        {
            if (pair->isConnected)
            {
                auto responseData = msg.c_str();
                sendMessage(*pair->connection, responseData, strlen(responseData), 0);
            }
        }
    }

    void server::broadcastToRoom(const char* responseData, size_t size, SOCKET cs)
    {
        auto room = roomManager.getRoom(getClient(cs).get());
        for (auto pair : room->clients())
        {
            if (pair->isConnected)
            {
                sendMessage(*pair->connection, responseData, size, 0);
            }
        }
    }

    int server::close()
    {
        if (!running)
        {
            logger::print("SERVER: server already closed.");
            return 1;
        }

        logger::print("SERVER: shutdown server . . .");
        closesocket(serverSocket);
        WSACleanup();

        initializing = false;
        running = false;
        return 0;
    }

    room* server::getRoom(int id)
    {
        return roomManager.getRoom(id);
    }

    void server::listening()
    {
        if (!running)
        {
            logger::printError("SERVER: server is not running. aborting listening thread");
            return;
        }

        isListening = true;

        char clientIP[INET_ADDRSTRLEN];

        logger::print("SERVER: waiting for connections . . .");
        initializing = false;
        while (running)
        {
            sockaddr_in clientAddr;
            int clientAddrSize = sizeof(clientAddr);

            SOCKET currentClientSocket = accept(serverSocket, reinterpret_cast<SOCKADDR*>(&clientAddr),
                                                &clientAddrSize);
            if (currentClientSocket == INVALID_SOCKET)
            {
                logger::printError("SERVER: Accept failed");
                error = true;
                close();
                break;
            }

            inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
            logger::print(
                (logger::getPrinter() << "SERVER: Connection accepted from " << clientIP << ":" << ntohs(
                    clientAddr.sin_port)).
                str());

            if (!running)
            {
                logger::printError("SERVER: server not running anymore, aborting listening thread");
                break;
            }

            std::thread clientThread([this, currentClientSocket]()
            {
                this->clientHandler(currentClientSocket);
            });
            clientThread.detach();
        }

        logger::print("SERVER: no longer waiting for connections.");
        isListening = false;
    }

    void server::disconnectClient(SOCKET clientSocket)
    {
        auto client = getClient(clientSocket);
        bool locked = roomManager.getRoom(client.get())->isLocked();
        roomManager.clientDisconnected(client.get());

        closesocket(clientSocket);
        auto it = clients.find(client->id);
        if (it != clients.end())
        {
            if (locked)
            {
                it->second->disconnect();
            }
            else
            {
                clients.erase(it);
            }
        }
    }

    void server::clientReconnected(const std::shared_ptr<clientInfo>& client, SOCKET uint)
    {
        client->connection = &uint;
        client->reconnect();

        if (onClientReconnected != nullptr)
        {
            onClientReconnected(client.get());
        }
    }

    void server::clientHandler(SOCKET clientSocket)
    {
        connectionsCount++;
        int id = connectionsCount;
        if (!validateKey(clientSocket, id))
        {
            logger::printError(
                (logger::getPrinter() << "SERVER: closing client connection [" << clientSocket << "] invalid Key").
                str());
            disconnectClient(clientSocket);
            return;
        }

        auto cl = getClientFromId(id);
        if (cl != nullptr)
        {
            clientReconnected(cl, clientSocket);
        }
        else
        {
            std::shared_ptr<clientInfo> client = std::make_shared<clientInfo>(connectionsCount);
            client->connection = &clientSocket;
            clients.insert(std::make_pair(id, client));
        }

        if (!running)
        {
            logger::printError("SERVER: server not running anymore, aborting client listening");
            return;
        }

        logger::print((logger::getPrinter() << "SERVER: Connection accepted from " << clientSocket << "").str());


        char recvData[1024];
        int recvSize;

        while (running && (recvSize = recv(clientSocket, recvData, sizeof(recvData), 0)) > 0)
        {
            recvData[recvSize] = '\0';
            logger::print((logger::getPrinter() << "SERVER: Received from client: " << recvData).str());

            std::string message;
            message.assign(recvData);

            std::vector<std::string> data = stringUtils::splitString(message);
            if (containsCommand(data[0]))
            {
                commands[data[0]](message, clientSocket);
            }

            if (containsCustomCommand(data[0]))
            {
                customCommands[data[0]](message, clientSocket);
            }
        }

        if (!running)
        {
            logger::printError("SERVER: server not running anymore, closing client thread");
            return;
        }

        if (recvSize == 0)
        {
            logger::print("SERVER: Client disconnected.");
        }
        else
        {
            logger::printError("SERVER: Receive failed");
        }

        logger::print((logger::getPrinter() << "SERVER: closing client connection [" << clientSocket << "]").str());
        disconnectClient(clientSocket);
    }

    bool server::containsCommand(const std::string& command)
    {
        auto it = commands.find(command);

        return it != commands.end();
    }

    bool server::containsCustomCommand(const std::string& command)
    {
        auto it = customCommands.find(command);

        return it != customCommands.end();
    }

    bool server::validateKey(SOCKET clientSocket, int& id) const
    {
        if (!running)
        {
            logger::printError("SERVER: server not running anymore, aborting validation");
            return false;
        }

        logger::print("SERVER: validating key . . .");
        char keyBuffer[20];
        int keySize = recv(clientSocket, keyBuffer, sizeof(keyBuffer), 0);
        if (keySize <= 0)
        {
            logger::printError("SERVER: Key receive failed");
            return false;
        }

        keyBuffer[keySize] = '\0';

        std::string keyReceived(keyBuffer);
        auto data = stringUtils::splitString(keyReceived);

        if (validKeys.find(data[0]) == validKeys.end())
        {
            logger::printError("SERVER: Invalid key");
            sendMessage(clientSocket, NC_INVALID_KEY, strlen(NC_INVALID_KEY), 0);
            return false;
        }

        if (data.size() > 1)
        {
            id = stoi(data[1]);
        }

        std::stringstream ss;
        ss << NC_VALID_KEY << NC_SEPARATOR << id;
        std::string str = ss.str();
        const char* response = str.c_str();
        sendMessage(clientSocket, response, strlen(response), 0);
        return true;
    }

    std::shared_ptr<clientInfo> server::getClient(SOCKET clientSocket)
    {
        for (auto pair : clients)
        {
            if (*pair.second->connection == clientSocket)
            {
                return clients[pair.first];
            }
        }

        return nullptr;
    }

    std::shared_ptr<clientInfo> server::getClientFromId(size_t id) const
    {
        for (auto pair : clients)
        {
            if (pair.first == id)
            {
                return clients.at(pair.first);
            }
        }

        return nullptr;
    }

    void server::createRoom(const std::string& message, SOCKET clientSocket)
    {
        std::vector<std::string> data = stringUtils::splitString(message);
        logger::print((logger::printer() << "SERVER: creating room [" << data[1] << "]").str());

        std::string roomName = data[1];
        int id = roomsCount;
        roomsCount++;
        roomManager.createRoom(id, roomName);
        roomManager.enterRoom(id, getClient(clientSocket));

        std::stringstream ss;
        ss << NC_CREATE_ROOM << NC_SEPARATOR;
        ss << roomManager.getRoomSerialized(id);
        std::string str = ss.str();
        const char* responseData = str.c_str();

        sendMessage(clientSocket, responseData, strlen(responseData), 0);
        logger::print((logger::getPrinter() << "SERVER: created room with id" << id << "").str());
    }

    void server::listRoom(const std::string& message, SOCKET clientSocket)
    {
        logger::print("SERVER: listing rooms");
        std::string str = roomManager.listRooms();
        const char* responseData = str.c_str();
        sendMessage(clientSocket, responseData, strlen(responseData), 0);
    }

    void server::getRoom(const std::string& message, SOCKET clientSocket)
    {
        std::vector<std::string> data = stringUtils::splitString(message);
        logger::print((logger::printer() << "SERVER: getting room [" << data[1] << "] information").str());
        int id = stoi(data[1]);
        std::stringstream ss;
        ss << NC_GET_ROOM << NC_SEPARATOR;
        ss << roomManager.getRoomSerialized(id);
        std::string str = ss.str();
        const char* responseData = str.c_str();
        sendMessage(clientSocket, responseData, strlen(responseData), 0);
        logger::print((logger::getPrinter() << "SERVER: sent to client room updated data [" << id << "]").str());
    }

    void server::enterRoom(const std::string& message, SOCKET clientSocket)
    {
        std::vector<std::string> data = stringUtils::splitString(message);
        logger::print((logger::printer() << "SERVER: client entering room [" << data[1] << "]").str());
        int id = stoi(data[1]);
        roomManager.enterRoom(id, getClient(clientSocket));

        std::stringstream ss;
        ss << NC_ENTER_ROOM << NC_SEPARATOR;
        ss << roomManager.getRoomSerialized(id);
        std::string str = ss.str();
        const char* responseData = str.c_str();

        broadcastToRoom(responseData, strlen(responseData), clientSocket);
        logger::print((logger::getPrinter() << "SERVER: added client to room with id [" << id << "]").str());
    }

    void server::exitRoom(const std::string& message, SOCKET clientSocket)
    {
        logger::print("SERVER: client exiting room");
        auto client = getClient(clientSocket);
        roomManager.exitRoom(client.get());
        const char* responseData = NC_EXIT_ROOM;
        sendMessage(clientSocket, responseData, strlen(responseData), 0);
    }

    void server::updateClientName(const std::string& message, SOCKET clientSocket)
    {
        std::vector<std::string> data = stringUtils::splitString(message);
        logger::print("SERVER: updating client name");
        auto client = getClient(clientSocket);
        client->name = data[1];
        const char* responseData = NC_SET_NAME;
        sendMessage(clientSocket, responseData, strlen(responseData), 0);
    }

    void server::getSeed(const std::string& message, SOCKET clientSocket)
    {
        logger::print("SERVER: getting seed");
        std::stringstream ss;
        ss << NC_GET_SEED << NC_SEPARATOR << seed;
        std::string str = ss.str();
        const char* responseData = str.c_str();
        sendMessage(clientSocket, responseData, strlen(responseData), 0);
    }

    void server::setSeed(const std::string& message, SOCKET clientSocket)
    {
        std::vector<std::string> data = stringUtils::splitString(message);
        this->seed = std::stoull(data[1]);
    }

    void server::sendMessage(SOCKET clientSocket, const char* responseData, int len, int flags) const
    {
        auto result = send(clientSocket, responseData, len, flags);
        if (result == SOCKET_ERROR)
        {
            logger::printError((logger::getPrinter() << "Failed to send message [" << result << "]").str());
        }
        if (result == 0)
        {
            logger::printError((logger::getPrinter() << "Connection closed [" << result << "]").str());
        }
    }
}
