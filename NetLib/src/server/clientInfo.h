#pragma once
#include <WS2tcpip.h>
#include <string>
#include "../../framework.h"

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
