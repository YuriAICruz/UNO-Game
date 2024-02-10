#include "server.h"

void server::clientHandler(SOCKET clientSocket)
{
    std::cout << "waiting for client data . . .\n";
    char recvData[1024];
    int recvSize;

    while ((recvSize = recv(clientSocket, recvData, sizeof(recvData), 0)) > 0) {
        recvData[recvSize] = '\0'; // Null-terminate received data
        std::cout << "Received from client: " << recvData << std::endl;

        std::cout << "sending automatic response\n";
        const char* responseData = "Message received by server!";
        send(clientSocket, responseData, strlen(responseData), 0);
        // Echo received data back to client
        //send(clientSocket, recvData, recvSize, 0);
    }

    if (recvSize == 0) {
        std::cout << "Client disconnected.\n";
    } else {
        std::cerr << "Receive failed\n";
    }

    std::cout << "closing client connection [" << clientSocket << "]\n";
    closesocket(clientSocket);
}
