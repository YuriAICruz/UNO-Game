#include "room.h"

#include <sstream>
#include <string>

#include "../serverCommands.h"
#include "../stringUtils.h"

int room::count()
{
    return connectedClients.size();
}

void room::addClient(const std::shared_ptr<clientInfo>& client)
{
    connectedClients.push_back(client);
}

void room::removeClient(clientInfo* client)
{
    int i = 0;
    for (int n = connectedClients.size(); i < n; ++i)
    {
        if (connectedClients.at(i).get() == client)
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

std::vector<clientInfo*> room::clients()
{
    std::vector<clientInfo*> list;
    list.resize(connectedClients.size());
    for (int i = 0, n = connectedClients.size(); i < n; ++i)
    {
        list[i] = connectedClients[i].get();
    }

    return list;
}

clientInfo* room::getClientByIndex(int index) const
{
    return connectedClients.at(index).get();
}

clientInfo* room::getClient(int clientId) const
{
    for (auto client : connectedClients)
    {
        if (client->getId() == clientId)
        {
            return client.get();
        }
    }

    return nullptr;
}

bool room::hasClient(clientInfo* client) const
{
    for (auto connectedClient : connectedClients)
    {
        if (connectedClient->id == client->id)
        {
            return true;
        }
    }

    return false;
}

std::string room::getRoomSerialized(int id)
{
    std::stringstream ss;
    ss << id << NC_SEPARATOR << getName() << NC_SEPARATOR;
    int i = 0;
    for (auto client : connectedClients)
    {
        if (i > 0)
        {
            ss << NC_SEPARATOR;
        }
        ss << client->getId();
        ss << NC_SEPARATOR;
        ss << client->getName();
        i++;
    }
    return ss.str();
}

room room::constructRoom(std::string data)
{
    auto splitData = stringUtils::splitString(data);
    int id = std::stoi(splitData[0]);
    std::string name = splitData[1];
    std::vector<clientInfo> clients;
    for (int i = 2, n = splitData.size(); i < n; i += 2)
    {
        int clientId = std::stoi(splitData[i]);
        std::string clientName = splitData[i + 1];
        clients.emplace_back(clientInfo{clientId, clientName});
    }

    return room(id, name, clients);
}
