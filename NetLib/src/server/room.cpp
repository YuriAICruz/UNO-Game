#include "room.h"

int room::count()
{
    return connectedClients.size();
}

void room::addClient(clientInfo* client)
{
    connectedClients.push_back(client);
}

void room::removeClient(clientInfo* client)
{
    int i = 0;
    for (int n = connectedClients.size(); i < n; ++i)
    {
        if (connectedClients.at(i) == client)
        {
            break;
        }
    }

    if (i >= connectedClients.size())
    {
        throw std::exception("client not fount");
    }

    connectedClients.erase(connectedClients.begin() + i);
}
