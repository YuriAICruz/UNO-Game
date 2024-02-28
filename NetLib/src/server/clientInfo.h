#pragma once

#include <string>

#include "../logger.h"
#include "../../framework.h"
#include "../winSockImp.h"

namespace netcode
{
    struct NETCODE_API clientInfo
    {
        int id;
        std::string name = "John Doe";
        SOCKET connection;
        bool isConnected = true;
        bool ready = false;

        clientInfo() = default;


        clientInfo(int id) : id(id)
        {
        }

        clientInfo(int id, const std::string& name) : clientInfo(id)
        {
            this->name = name;
        }

        int getId() const
        {
            return id;
        }

        std::string getName()
        {
            return name;
        }

        void setName(const std::string& newName)
        {
            name = newName;
        }

        void reconnect()
        {
            if (isConnected)
            {
                logger::printError(
                    (logger::getPrinter() << "Client is already connected [" << id << ":" << name << "]").str()
                );
            }
            isConnected = true;
        }

        void disconnect()
        {
            isConnected = false;
            connection = 0;
        }
    };
}
