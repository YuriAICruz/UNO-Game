#pragma once
#include <memory>
#include <vector>
#include <string>

#include "../../framework.h"
#include "clientInfo.h"

class NETCODE_API room
{
private:
    std::string name;
    std::vector<std::shared_ptr<clientInfo>> connectedClients;

public:
    room() = default;

    room(const std::string& name) : name(name)
    {
    }

    room(int id, const std::string& name, const std::vector<clientInfo>& clients) : name(name)
    {
        connectedClients.resize(clients.size());
        int i = 0;
        for (auto client : clients)
        {
            connectedClients[i] = std::make_shared<clientInfo>(client);
            i++;
        }
    }

    int count();
    void addClient(const std::shared_ptr<clientInfo>& client);
    void removeClient(clientInfo* client);
    clientInfo* getClient(int clientId) const;
    bool hasClient(clientInfo* client) const;

    std::string getRoomSerialized(int id);
    static room constructRoom(std::string data);
    static int headerSerializationElementsCount();

    std::string& getName()
    {
        return name;
    }
};
