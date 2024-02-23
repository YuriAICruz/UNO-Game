#include "room.h"

#include <sstream>
#include <string>

#include "../logger.h"
#include "../serverCommands.h"
#include "../stringUtils.h"

namespace netcode
{
    int room::count()
    {
        return connectedClients.size();
    }

    bool room::addClient(const std::shared_ptr<clientInfo>& client)
    {
        if (locked)
        {
            auto cl = getClient(client->id);
            if (cl != nullptr && !cl->isConnected)
            {
                cl->reconnect();
            }
            else
            {
                logger::printError("can't add new clients, room is locked");
                return false;
            }
        }
        connectedClients.push_back(client);
        return true;
    }

    void room::removeClient(clientInfo* client)
    {
        if (locked)
        {
            logger::printError("locked Room, can't remove client, it will be flagged as disconnected.");
            getClient(client->id)->disconnect();
            return;
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
            throw std::exception("client not found");
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

    bool room::hasClientConnection(SOCKET clientSocket) const
    {
        for (auto connectedClient : connectedClients)
        {
            if (connectedClient->connection != nullptr && *connectedClient->connection == clientSocket)
            {
                return true;
            }
        }

        return false;
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

    void room::setClientReady()
    {
        clientReady = true;
    }

    void room::setClientNotReady()
    {
        clientReady = false;
    }

    bool room::isClientReady() const
    {
        return clientReady;
    }

    std::string room::getRoomSerialized(int id)
    {
        std::stringstream ss;
        ss << id << NC_SEPARATOR << getName();
        int i = 0;
        for (auto client : connectedClients)
        {
            ss << NC_SEPARATOR;
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
}
