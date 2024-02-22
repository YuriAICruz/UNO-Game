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
                sendMessage(msg.c_str(), *pair.second->connection);
            }
        }
    }

    void server::broadcastToRoom(std::string msg, SOCKET cs)
    {
        auto room = roomManager.getRoom(getClient(cs).get());
        if (room == nullptr)
        {
            throw std::exception("Room Not found");
        }
        for (clientInfo* pair : room->clients())
        {
            if (pair->isConnected)
            {
                sendMessage(msg, *pair->connection);
            }
        }
    }

    void server::broadcastToRoomRaw(const char* responseData, size_t size, SOCKET cs)
    {
        auto room = roomManager.getRoom(getClient(cs).get());
        for (auto pair : room->clients())
        {
            if (pair->isConnected)
            {
                sendMessageRaw(*pair->connection, responseData, size, 0);
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

    void server::lockRoom(SOCKET cs)
    {
        roomManager.getRoom(getClient(cs).get())->lock();
    }

    void server::unlockRoom(SOCKET cs)
    {
        roomManager.getRoom(getClient(cs).get())->unlock();
    }

    bool server::isRoomReady(int roomId)
    {
        return roomManager.getRoom(roomId)->isClientReady();
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
        if (nullptr == client.get())
        {
            logger::print(
                (logger::getPrinter() << "SERVER: player with socket [" << clientSocket << "] could not be found").
                str());
            return;
        }

        auto room = roomManager.getRoom(client.get());
        bool locked = room == nullptr ? false : room->isLocked();
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
            std::shared_ptr<clientInfo> client = std::make_shared<clientInfo>(id);
            client->connection = &clientSocket;
            clients.insert(std::make_pair(id, client));
        }

        logger::print((logger::getPrinter() << "SERVER: client connected [" << id << "]").str());

        if (!running)
        {
            logger::printError("SERVER: server not running anymore, aborting client listening");
            return;
        }

        logger::print((logger::getPrinter() << "SERVER: Connection accepted from " << clientSocket << "").str());


        char recvData[NC_PACKET_SIZE];
        int recvSize;

        while (running && (recvSize = recv(clientSocket, recvData, sizeof(recvData), 0)) > 0)
        {
            recvData[recvSize] = '\0';
            logger::print((logger::getPrinter() << "SERVER: Received from client: " << recvData).str());

            std::string message;
            message.assign(recvData);

            auto commandsBuffer = stringUtils::splitString(message, NC_COMMAND_END);
            for (auto& command : commandsBuffer)
            {
                std::vector<std::string> data = stringUtils::splitString(command);
                if (containsCommand(data[0]))
                {
                    commands[data[0]](command, clientSocket);
                }

                if (containsCustomCommand(data[0]))
                {
                    customCommands[data[0]](command, clientSocket);
                }
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
        auto commandsBuffer = stringUtils::splitString(keyReceived, NC_COMMAND_END);
        for (auto& command : commandsBuffer)
        {
            auto data = stringUtils::splitString(command);

            if (validKeys.find(data[0]) == validKeys.end())
            {
                logger::printError("SERVER: Invalid key");
                sendMessage(NC_INVALID_KEY, clientSocket);
                return false;
            }

            if (data.size() > 1)
            {
                id = stoi(data[1]);
            }

            std::stringstream ss;
            ss << NC_VALID_KEY << NC_SEPARATOR << id;
            sendMessage(ss.str(), clientSocket);
            return true;
        }

        return false;
    }

    std::shared_ptr<clientInfo> server::getClient(SOCKET clientSocket)
    {
        for (auto& pair : clients)
        {
            if (pair.second->connection != nullptr && *pair.second->connection == clientSocket)
            {
                return clients[pair.first];
            }
        }

        return nullptr;
    }

    std::shared_ptr<clientInfo> server::getClientFromId(size_t id) const
    {
        for (auto& pair : clients)
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
        sendMessage(ss.str(), clientSocket);

        logger::print((logger::getPrinter() << "SERVER: created room with id" << id << "").str());

        if (onRoomCreated != nullptr)
        {
            onRoomCreated(roomManager.getRoom(id));
        }
    }

    void server::listRoom(const std::string& message, SOCKET clientSocket)
    {
        logger::print("SERVER: listing rooms");
        std::string str = roomManager.listRooms();
        sendMessage(str, clientSocket);
    }

    void server::getRoom(const std::string& message, SOCKET clientSocket)
    {
        std::vector<std::string> data = stringUtils::splitString(message);
        logger::print((logger::printer() << "SERVER: getting room [" << data[1] << "] information").str());
        int id = stoi(data[1]);

        std::stringstream ss;
        ss << NC_GET_ROOM << NC_SEPARATOR;
        ss << roomManager.getRoomSerialized(id);
        sendMessage(ss.str(), clientSocket);

        logger::print((logger::getPrinter() << "SERVER: sent to client room updated data [" << id << "]").str());
    }

    void server::broadcastUpdatedRoom(SOCKET clientSocket)
    {
        auto room = roomManager.getRoom(getClient(clientSocket).get());
        logger::print(
            (logger::printer() << "SERVER: broadcasting updated room data to all clients [" << room->getId() << "]").
            str());
        int id = room->getId();

        std::stringstream ss;
        ss << NC_GET_ROOM << NC_SEPARATOR;
        ss << roomManager.getRoomSerialized(id);
        broadcastToRoom(ss.str(), clientSocket);

        logger::print((logger::getPrinter() << "SERVER: sent to all clients in room [" << id << "]").str());
    }

    void server::sendRoomData(SOCKET clientSocket, int id)
    {
        std::stringstream ss;
        ss << NC_ENTER_ROOM << NC_SEPARATOR;
        ss << roomManager.getRoomSerialized(id);

        broadcastToRoom(ss.str(), clientSocket);
    }

    void server::enterRoom(const std::string& message, SOCKET clientSocket)
    {
        std::vector<std::string> data = stringUtils::splitString(message);
        logger::print((logger::printer() << "SERVER: client entering room [" << data[1] << "]").str());
        int id = stoi(data[1]);
        if (roomManager.enterRoom(id, getClient(clientSocket)))
        {
            sendRoomData(clientSocket, id);
            logger::print((logger::getPrinter() << "SERVER: added client to room with id [" << id << "]").str());
            return;
        }

        logger::print((logger::getPrinter() << "SERVER: Could not add client to room [" << id << "]").str());
        sendMessage(NC_EXIT_ROOM, clientSocket);
    }

    void server::exitRoom(const std::string& message, SOCKET clientSocket)
    {
        logger::print("SERVER: client exiting room");

        auto client = getClient(clientSocket);
        if (nullptr == client.get())
        {
            logger::printError(
                (logger::getPrinter() << "Could not find client with socket [" << clientSocket << "]").str()
            );
        }
        else
        {
            roomManager.exitRoom(client.get());
        }

        sendMessage(NC_EXIT_ROOM, clientSocket);
    }

    void server::updateRoomStatus(const std::string& message, SOCKET clientSocket)
    {
        auto data = stringUtils::splitString(message);
        bool ready = data[1] == "1";

        std::stringstream ss;
        ss << NC_ROOM_READY_STATUS << NC_SEPARATOR;

        auto client = getClient(clientSocket).get();

        auto room = roomManager.getRoom(client);
        if (room == nullptr || room->isLocked())
        {
            ss << 0;
            broadcastToRoom(ss.str(), clientSocket);
            return;
        }

        client->ready = ready;

        ss << 1;
        broadcastToRoom(ss.str(), clientSocket);

        if (roomManager.roomClientsAreReady(room->getId()))
        {
            broadcastToRoom(NC_ROOM_ALL_READY, clientSocket);
        }
        else
        {
            broadcastToRoom(NC_ROOM_NOT_READY, clientSocket);
        }
    }

    void server::updateClientName(const std::string& message, SOCKET clientSocket)
    {
        std::vector<std::string> data = stringUtils::splitString(message);
        logger::print("SERVER: updating client name");
        auto client = getClient(clientSocket);
        client->name = data[1];
        sendMessage(NC_SET_NAME, clientSocket);
    }

    void server::getSeed(const std::string& message, SOCKET clientSocket)
    {
        logger::print("SERVER: getting seed");
        std::stringstream ss;
        ss << NC_GET_SEED << NC_SEPARATOR << seed;
        std::string str = ss.str();
        const char* responseData = str.c_str();
        sendMessage(responseData, clientSocket);
    }

    void server::setSeed(const std::string& message, SOCKET clientSocket)
    {
        std::vector<std::string> data = stringUtils::splitString(message);
        this->seed = std::stoull(data[1]);
    }

    void server::sendMessage(std::string message, SOCKET clientSocket) const
    {
        sendMessage(message.c_str(), clientSocket);
    }

    void server::sendMessage(const char* message, SOCKET clientSocket) const
    {
        std::stringstream ss;
        ss << message << NC_COMMAND_END;
        std::string str = ss.str();
        const char* response = str.c_str();
        logger::print((logger::getPrinter() << "SERVER: message: " << response).str());
        sendMessageRaw(clientSocket, response, strlen(response), 0);
    }

    void server::sendMessageRaw(SOCKET clientSocket, const char* responseData, int len, int flags) const
    {
        if(len<100)
        {
            logger::print(
                (logger::getPrinter() << "SERVER: sending message size [" << responseData << "]" << " to connection [" <<
                    clientSocket << "]").str());
        }else
        {
            logger::print(
                (logger::getPrinter() << "SERVER: sending message size [" << len << "]" << " to connection [" <<
                    clientSocket << "]").str());   
        }
        auto result = send(clientSocket, responseData, len, flags);
        if (result == SOCKET_ERROR)
        {
            logger::printError((logger::getPrinter() << "SERVER: Failed to send message [" << result << "]").str());
        }
        if (result == 0)
        {
            logger::printError((logger::getPrinter() << "SERVER: Connection closed [" << result << "]").str());
        }
    }
}
