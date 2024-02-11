#include "server.h"
#include <iostream>
#include <WS2tcpip.h>
#include <thread>
#include <sstream>
#include "../serverCommands.h"

int server::start(int port)
{
    std::cout << "starting server . . .\n";
    std::cout << "initializing Winsock . . .\n";
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
    std::cout << "Winsock initialized\n";

    std::cout << "creating socket . . .\n";
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }
    std::cout << "socket created\n";

    std::cout << "binding socket . . .\n";
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "bind success to port: " << port << "\n";

    std::cout << "setup listening . . .\n";
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port " << serverAddr.sin_port << "...\n";

    std::stringstream ss;
    ss << "start cmd /c .\\ngrok.exe tcp " << port;
    ngrokPID = std::system(ss.str().c_str());

    char clientIP[INET_ADDRSTRLEN];

    std::cout << "waiting for connections . . .\n";
    while (true)
    {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        SOCKET currentClientSocket = accept(serverSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrSize);
        if (currentClientSocket == INVALID_SOCKET)
        {
            std::cerr << "Accept failed\n";
            int result = close();
            return result + 1;
        }

        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        std::cout << "Connection accepted from " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;

        std::thread clientThread([this, currentClientSocket]()
        {
            this->clientHandler(currentClientSocket);
        });
        clientThread.detach();
    }
}

void server::broadcast(std::string msg)
{
    for (auto client : clients)
    {
        auto responseData = msg.c_str();
        send(*client.second.connection, responseData, strlen(responseData), 0);
    }
}

int server::close() const
{
    std::cout << "shutdown server . . .\n";
    closesocket(serverSocket);
    WSACleanup();

    std::cout << "terminating ngrok...\n";
    std::string killCommand = "taskkill /PID " + std::to_string(ngrokPID) + " /F";
    system(killCommand.c_str());
    return 0;
}

void server::clientHandler(SOCKET clientSocket)
{
    if (!validateKey(clientSocket))
    {
        std::cerr << "closing client connection [" << clientSocket << "] invalid Key\n";
        closesocket(clientSocket);
        clients.erase(connectionsCount);
        return;
    }

    std::cout << "Connection accepted from " << clientSocket << "\n";

    clientInfo client = clientInfo();
    client.connection = &clientSocket;
    clients.insert(std::make_pair(connectionsCount, client));
    connectionsCount++;

    char recvData[1024];
    int recvSize;

    while ((recvSize = recv(clientSocket, recvData, sizeof(recvData), 0)) > 0)
    {
        recvData[recvSize] = '\0';
        std::cout << "Received from client: " << recvData << std::endl;

        std::string message;
        message.assign(recvData);

        filterCommands(message, clientSocket);
    }

    if (recvSize == 0)
    {
        std::cout << "Client disconnected.\n";
    }
    else
    {
        std::cerr << "Receive failed\n";
    }

    std::cout << "closing client connection [" << clientSocket << "]\n";
    closesocket(clientSocket);
    clients.erase(connectionsCount);
}

bool server::validateKey(SOCKET clientSocket)
{
    std::cerr << "validating key . . .\n";
    char keyBuffer[13];
    int keySize = recv(clientSocket, keyBuffer, sizeof(keyBuffer), 0);
    if (keySize <= 0)
    {
        std::cerr << "Key receive failed\n";
        return false;
    }

    if (keySize != sizeof(keyBuffer) - 1)
    {
        std::cerr << "Invalid key\n";
        return false;
    }

    keyBuffer[keySize] = '\0';

    std::string keyReceived(keyBuffer);
    if (validKeys.find(keyReceived) == validKeys.end())
    {
        std::cerr << "Invalid key\n";
        return false;
    }

    return true;
}

clientInfo* server::getClient(SOCKET clientSocket)
{
    for (auto pair : clients)
    {
        if (*pair.second.connection == clientSocket)
        {
            return &clients[pair.first];
        }
    }

    return nullptr;
}

void server::filterCommands(std::string& message, SOCKET clientSocket)
{
    if (message == NC_CREATE_ROOM)
    {
        std::cout << "creating room\n";
        int id = createRoom();
        const char* responseData = std::to_string(id).c_str();
        rooms[id].addClient(getClient(clientSocket));
        send(clientSocket, responseData, strlen(responseData), 0);
        std::cout << "created room with id" << id << "\n";
        broadcast("This message is for all!!");
    }
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
        std::cout << "listed all rooms\n";
    }

    std::vector<std::string> data = splitString(message);
    if (data.size() == 2)
    {
        if (data[0] == NC_ENTER_ROOM)
        {
            int id = std::stoi(data[1]);
            auto client = getClient(clientSocket);
            rooms[id].addClient(client);
            const char* responseData = NC_SUCCESS;
            send(clientSocket, responseData, strlen(responseData), 0);
            std::cout << "client " << client->connection << " added to room " << id << "\n";
        }
        if (data[0] == NC_EXIT_ROOM)
        {
            int id = std::stoi(data[1]);
            auto client = getClient(clientSocket);
            rooms[id].removeClient(client);
            const char* responseData = NC_SUCCESS;
            send(clientSocket, responseData, strlen(responseData), 0);
            std::cout << "client " << client->connection << " removed from room " << id << "\n";
        }
    }
}

int server::createRoom()
{
    auto r = room();
    rooms.insert(std::make_pair(connectionsCount, r));
    roomsCount++;

    return connectionsCount--;
}

room* server::getRoom(int id)
{
    return &rooms[id];
}

std::vector<std::string> server::splitString(const std::string& s)
{
    std::vector<std::string> tokens;
    std::istringstream ss(s);
    std::string token;

    while (std::getline(ss, token, NC_SEPARATOR))
    {
        tokens.push_back(token);
    }

    return tokens;
}
