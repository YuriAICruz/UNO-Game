#pragma once
#include <map>

#include "clientInfo.h"
#include "room.h"

class roomManager
{
private:
    std::map<int, room> rooms;

public:
    void createRoom(int id, std::string roomName);
    room* getRoom(int id);
    room* getRoom(clientInfo* client);
    void exitRoom(clientInfo* client);
    void enterRoom(int id, const std::shared_ptr<clientInfo>& client);
    std::string listRooms() const;
    std::string getRoomSerialized(int id);
};
