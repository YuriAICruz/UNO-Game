#include "room.h"

#include <sstream>
#include <string>

#include "../serverCommands.h"
#include "../stringUtils.h"

namespace netcode
{
    int room::count()
    {
        return connectedClients.size();
    }

    void room::addClient(const std::shared_ptr<clientInfo>& client)
    {
        if (locked)
        {
            throw std::exception("can't add new clients, room is locked");
        }
        connectedClients.push_back(client);
    }

    void room::removeClient(clientInfo* client)
    {
        if (locked)
        {
            throw std::exception("can't remove clients, room is locked");
        }

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

    void room::lock()
    {
        locked = true;
    }

    void room::unlock()
    {
        locked = false;
    }

    bool room::isLocked() const
    {
        return locked;
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

    std::string room::getClientName(uint16_t id) const
    {
        return getClient(id)->name;
    }

    std::vector<std::string> room::getClientsNames()
    {
        std::vector<std::string> names;
        for (auto& client : clients())
        {
            names.emplace_back(client->name);
        }
        return names;
    }

    std::vector<uint16_t> room::getClientsIds()
    {
        std::vector<std::uint16_t> ids;
        for (auto& client : clients())
        {
            ids.emplace_back(client->id);
        }
        return ids;
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
}
