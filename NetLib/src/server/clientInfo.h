#pragma once

#ifndef NET_WinSock2
#define NET_WinSock2
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <string>
#include "../../framework.h"

namespace netcode
{
    struct NETCODE_API clientInfo
    {
        int id;
        std::string name = "John Doe";
        SOCKET* connection;

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
    };
}
