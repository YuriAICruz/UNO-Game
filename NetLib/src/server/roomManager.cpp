﻿#include <sstream>

#include "roomManager.h"
#include "../serverCommands.h"


void roomManager::createRoom(int id, std::string roomName)
{
    auto r = room(id, roomName);
    rooms.insert(std::make_pair(id, r));
}

room* roomManager::getRoom(int id)
{
    return &rooms[id];
}

room* roomManager::getRoom(clientInfo* client)
{
    int index = -1;
    for (auto pair : rooms)
    {
        if (pair.second.hasClient(client))
        {
            index = pair.first;
            break;
        }
    }

    if (index < 0)
    {
        return nullptr;
    }

    return &rooms[index];
}

void roomManager::exitRoom(clientInfo* client)
{
    auto room = getRoom(client);
    room->removeClient(client);
}

void roomManager::enterRoom(int id, const std::shared_ptr<clientInfo>& client)
{
    room* c = getRoom(client.get());
    if (c != nullptr)
    {
        exitRoom(client.get());
    }
    rooms[id].addClient(client);
}

std::string roomManager::listRooms() const
{
    std::stringstream ss;
    ss << NC_LIST_ROOMS << NC_SEPARATOR << rooms.size() << NC_SEPARATOR;

    int i = 0;
    for (auto pair : rooms)
    {
        if (i > 0)
        {
            ss << NC_OBJECT_SEPARATOR << NC_SEPARATOR;
        }
        ss << pair.second.getRoomSerialized(pair.first);
        i++;
    }

    return ss.str();
}

std::string roomManager::getRoomSerialized(int id)
{
    return rooms[id].getRoomSerialized(id);
}