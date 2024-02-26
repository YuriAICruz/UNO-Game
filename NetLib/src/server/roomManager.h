#pragma once
#include <map>

#include "clientInfo.h"
#include "room.h"

namespace netcode
{
    class roomManager
    {
    private:
        std::map<int, room> rooms;
        uint16_t count = 0;

    public:
        void createRoom(int id, std::string roomName);
        room* getRoom(int id);
        room* getRoom(clientInfo* client);
        void exitRoom(clientInfo* client);
        bool enterRoom(int id, const std::shared_ptr<clientInfo>& client);
        uint16_t roomsCount() const;
        std::string listRooms(const std::string& cmdKey) const;
        std::string getRoomSerialized(int id);
        void clientDisconnected(clientInfo* client);
        bool roomClientsAreReady(int roomId);
    };
}