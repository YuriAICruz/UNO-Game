#include "client/client.h"
#include "gtest/gtest.h"
#include "server/room.h"

TEST(RoomTests, AddAndRemove)
{
    auto r = room("MyRoom");

    auto client = std::make_shared<clientInfo>(0);
    r.addClient(client);

    EXPECT_EQ(1, r.count());

    auto clientB = std::make_shared<clientInfo>(1);
    r.addClient(clientB);

    EXPECT_EQ(2, r.count());

    r.removeClient(client.get());

    EXPECT_EQ(1, r.count());

    EXPECT_EQ(clientB->id, r.getClient(clientB->getId())->id);

    r.removeClient(clientB.get());

    EXPECT_EQ(0, r.count());
}

TEST(RoomTests, Serialization)
{
    std::string roomName = "MyRoom";
    int clientId = 12;
    auto r = room(roomName);
    auto client = std::make_shared<clientInfo>(clientId);
    std::string clientName = "MyClient";
    client->setName(clientName);
    r.addClient(client);

    auto str = r.getRoomSerialized(0);
    auto room = r.constructRoom(str);
    EXPECT_EQ(1, room.count());
    EXPECT_EQ(roomName, room.getName());
    auto c = room.getClient(clientId);
    EXPECT_EQ(clientName, c->name);
    EXPECT_EQ(clientId, c->id);
}
