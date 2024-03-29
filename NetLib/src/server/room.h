﻿#pragma once
#include <memory>
#include <vector>
#include <string>

#include "../../framework.h"
#include "../winSockImp.h"
#include "clientInfo.h"

namespace netcode
{
    class NETCODE_API room
    {
    private:
        std::string name;
        uint16_t id;
        bool locked = false;
        std::vector<std::shared_ptr<clientInfo>> connectedClients;
        bool clientReady = false;

    public:
        room() = default;

        room(uint16_t id, const std::string& name) : id(id), name(name)
        {
        }

        room(uint16_t id, const std::string& name, const std::vector<clientInfo>& clients) : id(id), name(name)
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
        bool addClient(const std::shared_ptr<clientInfo>& client);
        void removeClient(clientInfo* client);
        void lock();
        void unlock();
        bool isLocked() const;
        std::vector<clientInfo*> clients();
        clientInfo* getClientByIndex(int index) const;
        clientInfo* getClient(int clientId) const;
        bool hasClient(clientInfo* client) const;
        bool hasClientConnection(SOCKET clientSocket) const;

        std::string getRoomSerialized(int id);
        std::string getClientName(uint16_t id) const;
        std::vector<std::string> getClientsNames();
        std::vector<uint16_t> getClientsIds();
        void setClientReady();
        void setClientNotReady();
        void setClientsNotReady();
        bool isClientReady() const;
        static bool constructRoom(std::string data, room& result);
        static bool constructRoom(std::vector<std::string> splitData, room& result);

        std::string getName() const
        {
            return name;
        }

        uint16_t getId() const
        {
            return id;
        }
    };
}