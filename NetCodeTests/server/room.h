#pragma once
#include <vector>

#include "clientInfo.h"

class room
{
private:
    std::vector<clientInfo*> connectedClients;
public:
    int count();
    void addClient(clientInfo*client);
    void removeClient(clientInfo*client);
};
