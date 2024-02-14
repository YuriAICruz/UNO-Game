#include <tuple>

#include "client/client.h"
#include "gtest/gtest.h"
#include "server/server.h"
#include "logger.h"

bool running;

std::tuple<std::shared_ptr<client>, std::shared_ptr<server>> startAndConnectClient()
{
    logger::print("TEST: startAndConnectClient");
    auto cl = std::make_shared<client>();
    auto sv = std::make_shared<server>();

    cl->start();
    EXPECT_TRUE(cl->isRunning());

    sv->start();
    while (!sv->isRunning() && !sv->hasError())
    {
    }
    EXPECT_TRUE(sv->isRunning());

    cl->connectToServer();
    while (!cl->isConnected() && !cl->hasError())
    {
    }
    EXPECT_TRUE(cl->isConnected());
    return std::tuple<std::shared_ptr<client>, std::shared_ptr<server>>(cl, sv);
}

void closeClient(client* cl, server* sv)
{
    logger::print("TEST: closeClient");
    cl->close();
    while (cl->isConnected() || cl->isRunning())
    {
    }
    EXPECT_FALSE(cl->isConnected());
    EXPECT_FALSE(cl->isRunning());
    sv->close();
    while (sv->isRunning())
    {
    }
    EXPECT_FALSE(sv->isRunning());
}

TEST(ClientTests, Create)
{
    auto cl = std::make_unique<client>();
    cl->start();

    EXPECT_TRUE(cl->isRunning());

    cl->close();
    while (cl->isConnected() || cl->isRunning())
    {
    }

    EXPECT_FALSE(cl->isConnected());
    EXPECT_FALSE(cl->isRunning());
}

TEST(ClientTests, Close)
{
    auto cl = std::make_unique<client>();

    cl->start();
    EXPECT_TRUE(cl->isRunning());

    cl->close();
    while (cl->isConnected() || cl->isRunning())
    {
    }
    EXPECT_FALSE(cl->isConnected());
    EXPECT_FALSE(cl->isRunning());
}

TEST(ClientTests, Connect)
{
    auto t = startAndConnectClient();
    auto cl = std::get<0>(t);
    auto sv = std::get<1>(t);
    closeClient(cl.get(), sv.get());
}

TEST(ClientTests, RequestRoomCreaion)
{
    auto t = startAndConnectClient();
    auto cl = std::get<0>(t);
    auto sv = std::get<1>(t);

    std::string roomName = "MyRoom";
    cl->createRoom(roomName);

    while (!cl->hasRoom() && !cl->hasError())
    {
    }

    EXPECT_TRUE(cl->hasRoom());
    EXPECT_EQ(roomName, cl->getRoomName());

    closeClient(cl.get(), sv.get());
}

TEST(ClientTests, ExitRoom)
{
    auto t = startAndConnectClient();
    auto cl = std::get<0>(t);
    auto sv = std::get<1>(t);

    std::string roomName = "MyRoom";
    cl->createRoom(roomName);

    while (!cl->hasRoom() && !cl->hasError())
    {
    }

    EXPECT_TRUE(cl->hasRoom());
    cl->exitRoom();
    while (cl->hasRoom() && !cl->hasError())
    {
    }
    EXPECT_FALSE(cl->hasRoom());

    closeClient(cl.get(), sv.get());
}

TEST(ClientTests, ListRooms)
{
    auto t = startAndConnectClient();
    auto cl = std::get<0>(t);
    auto sv = std::get<1>(t);

    std::stringstream ss;
    int roomsCount = 3;
    for (int i = 0; i < roomsCount; ++i)
    {
        ss << "Room " << i;
        std::string roomName = ss.str();
        cl->createRoom(roomName);

        while (!cl->hasRoom() && !cl->hasError())
        {
        }

        EXPECT_TRUE(cl->hasRoom());
        cl->exitRoom();
        while (cl->hasRoom() && !cl->hasError())
        {
        }
        EXPECT_FALSE(cl->hasRoom());
    }

    running = true;
    cl->getRooms([this,roomsCount](std::vector<room> rooms)
    {
        for (room r : rooms)
        {
            std::cout << "Room Name: " << r.getName() << "\n";
        }
        running = false;
        EXPECT_EQ(roomsCount, rooms.size());
    });

    while (running)
    {
    }

    closeClient(cl.get(), sv.get());
}

TEST(ClientTests, EnterRoom)
{
    auto t = startAndConnectClient();
    auto cl = std::get<0>(t);
    auto sv = std::get<1>(t);
    closeClient(cl.get(), sv.get());
}
