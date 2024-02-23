#pragma once

#ifndef NET_WinSock2
#define NET_WinSock2
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <string>

#include "../logger.h"
#include "../../framework.h"

namespace netcode
{
    struct NETCODE_API clientInfo
    {
        int id;
        std::string name = "John Doe";
        SOCKET* connection;
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
            connection = nullptr;
        }
    };
}
