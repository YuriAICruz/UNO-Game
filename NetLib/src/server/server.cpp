#include "server.h"
#include <WS2tcpip.h>
#include <thread>
#include <sstream>

#include "../logger.h"
#include "../serverCommands.h"
#include "../stringUtils.h"

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

    std::stringstream ss;
    ss << "start cmd /c .\\ngrok.exe tcp " << port;
    ngrokPID = std::system(ss.str().c_str());

    initializing = true;
    running = true;

    std::thread serverListeningThread([this]()
    {
        this->listening();
    });
    serverListeningThread.detach();
}

void server::broadcast(std::string msg)
{
    for (auto pair : clients)
    {
        auto responseData = msg.c_str();
        send(*pair.second->connection, responseData, strlen(responseData), 0);
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

    if (ngrokPID > 0)
    {
        logger::print("SERVER: terminating ngrok...");
        std::string killCommand = "taskkill /PID " + std::to_string(ngrokPID) + " /F";
        system(killCommand.c_str());
    }

    initializing = false;
    running = false;
    return 0;
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

        SOCKET currentClientSocket = accept(serverSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrSize);
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

void server::clientHandler(SOCKET clientSocket)
{
    if (!validateKey(clientSocket))
    {
        logger::printError(
            (logger::getPrinter() << "SERVER: closing client connection [" << clientSocket << "] invalid Key").str());
        closesocket(clientSocket);
        clients.erase(connectionsCount);
        return;
    }

    if (!running)
    {
        logger::printError("SERVER: server not running anymore, aborting client listening");
        return;
    }

    logger::print((logger::getPrinter() << "SERVER: Connection accepted from " << clientSocket << "").str());

    std::shared_ptr<clientInfo> client = std::make_shared<clientInfo>(connectionsCount);
    client->connection = &clientSocket;
    clients.insert(std::make_pair(connectionsCount, client));
    connectionsCount++;

    char recvData[1024];
    int recvSize;

    while (running && (recvSize = recv(clientSocket, recvData, sizeof(recvData), 0)) > 0)
    {
        recvData[recvSize] = '\0';
        logger::print((logger::getPrinter() << "SERVER: Received from client: " << recvData).str());

        std::string message;
        message.assign(recvData);

        filterCommands(message, clientSocket);
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
    closesocket(clientSocket);
    clients.erase(connectionsCount);
}

bool server::validateKey(SOCKET clientSocket)
{
    if (!running)
    {
        logger::printError("SERVER: server not running anymore, aborting validation");
        return 1;
    }

    logger::print("SERVER: validating key . . .");
    char keyBuffer[13];
    int keySize = recv(clientSocket, keyBuffer, sizeof(keyBuffer), 0);
    if (keySize <= 0)
    {
        logger::printError("SERVER: Key receive failed");
        return false;
    }

    if (keySize != sizeof(keyBuffer) - 1)
    {
        logger::printError("SERVER: Invalid key");
        return false;
    }

    keyBuffer[keySize] = '\0';

    std::string keyReceived(keyBuffer);
    if (validKeys.find(keyReceived) == validKeys.end())
    {
        logger::printError("SERVER: Invalid key");
        send(clientSocket, NC_INVALID_KEY, strlen(NC_INVALID_KEY), 0);
        return false;
    }

    send(clientSocket, NC_VALID_KEY, strlen(NC_VALID_KEY), 0);
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

void server::filterCommands(std::string& message, SOCKET clientSocket)
{
    if (message == NC_LIST_ROOMS)
    {
        std::stringstream ss;
        int i = 0;
        for (auto roomData : rooms)
        {
            if (i > 0)
            {
                ss << NC_SEPARATOR;
            }

            ss << roomData.first << "-" << roomData.second.count();
            i++;
        }
        const char* responseData = ss.str().c_str();
        send(clientSocket, responseData, strlen(responseData), 0);
        logger::print("SERVER: listed all rooms");
        return;
    }
    if (message == NC_EXIT_ROOM)
    {
        auto client = getClient(clientSocket);
        auto room = getRoom(client.get());
        room->removeClient(client.get());
        const char* responseData = NC_EXIT_ROOM;
        send(clientSocket, responseData, strlen(responseData), 0);
    }

    std::vector<std::string> data = stringUtils::splitString(message);
    if (data.size() == 2)
    {
        if (data[0] == NC_ENTER_ROOM)
        {
            int id = std::stoi(data[1]);
            auto client = getClient(clientSocket);
            rooms[id].addClient(client);
            const char* responseData = NC_SUCCESS;
            send(clientSocket, responseData, strlen(responseData), 0);
            logger::print(
                (logger::getPrinter() << "SERVER: client " << client->connection << " added to room " << id << "").
                str());
        }
        if (data[0] == NC_EXIT_ROOM)
        {
            int id = std::stoi(data[1]);
            auto client = getClient(clientSocket);
            rooms[id].removeClient(client.get());
            const char* responseData = NC_SUCCESS;
            send(clientSocket, responseData, strlen(responseData), 0);
            logger::print(
                (logger::getPrinter() << "SERVER: client " << client->connection << " removed from room " << id << "").
                str());
        }
        if (data[0] == NC_CREATE_ROOM)
        {
            logger::print((logger::printer() << "SERVER: creating room [" << data[1] << "]").str());
            int id = createRoom(data[1]);
            rooms[id].addClient(getClient(clientSocket));

            std::stringstream ss;
            ss << NC_CREATE_ROOM << NC_SEPARATOR;
            ss << rooms[id].getRoomSerialized(id);
            std::string str = ss.str();
            const char* responseData = str.c_str();

            send(clientSocket, responseData, strlen(responseData), 0);
            logger::print((logger::getPrinter() << "SERVER: created room with id" << id << "").str());
        }
    }
}

int server::createRoom(std::string roomName)
{
    auto r = room(roomName);
    rooms.insert(std::make_pair(connectionsCount, r));
    roomsCount++;

    return connectionsCount--;
}

room* server::getRoom(int id)
{
    return &rooms[id];
}

room* server::getRoom(clientInfo* client)
{
    int index = -1;
    for (auto pair : rooms)
    {
        if(pair.second.hasClient(client))
        {
            index = pair.first;
            break;
        }
    }

    if(index<0)
    {
        return nullptr;
    }

    return &rooms[index];
}
