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
    int id;
    std::vector<std::shared_ptr<clientInfo>> connectedClients;

public:
    room() = default;

    room(int id, const std::string& name) : id(id), name(name)
    {
    }

    room(int id, const std::string& name, const std::vector<clientInfo>& clients) : id(id), name(name)
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
    std::vector<clientInfo*> clients();
    clientInfo* getClientByIndex(int index) const;
    clientInfo* getClient(int clientId) const;
    bool hasClient(clientInfo* client) const;

    std::string getRoomSerialized(int id);
    static room constructRoom(std::string data);

    std::string& getName()
    {
        return name;
    }

    int getId()
    {
        return id;
    }
};
