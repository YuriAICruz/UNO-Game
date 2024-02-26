#include <tuple>

#include "client/client.h"
#include "gtest/gtest.h"
#include "server/server.h"
#include "logger.h"
#include "commands/client/createRoomCmd.h"
#include "commands/client/enterRoomCmd.h"
#include "commands/client/exitRoomCmd.h"
#include "commands/client/getRoomsCmd.h"
#include "commands/client/getRoomCmd.h"
#include "commands/client/hasRoomCmd.h"

bool running;

std::tuple<std::shared_ptr<netcode::client>, std::shared_ptr<netcode::server>> startAndConnectClient()
{
    logger::print("TEST: startAndConnectClient");
    auto cl = std::make_shared<netcode::client>();
    auto sv = std::make_shared<netcode::server>();

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
    return std::tuple<std::shared_ptr<netcode::client>, std::shared_ptr<netcode::server>>(cl, sv);
}

void closeClient(netcode::client* cl, netcode::server* sv)
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
    auto cl = std::make_unique<netcode::client>();
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
    auto cl = std::make_unique<netcode::client>();

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
    cl->executeCommand<commands::createRoomCmd>(roomName);

    EXPECT_TRUE(cl->executeCommand<commands::hasRoomCmd>());
    netcode::room* r;
    cl->executeCommand<commands::getRoomCmd>(r);

    EXPECT_EQ(roomName, r->getName());
    closeClient(cl.get(), sv.get());
}

TEST(ClientTests, ExitRoom)
{
    auto t = startAndConnectClient();
    auto cl = std::get<0>(t);
    auto sv = std::get<1>(t);

    std::string roomName = "MyRoom";
    cl->executeCommand<commands::createRoomCmd>(roomName);

    EXPECT_TRUE(cl->executeCommand<commands::hasRoomCmd>());
    cl->executeCommand<commands::exitRoomCmd>();
    EXPECT_FALSE(cl->executeCommand<commands::hasRoomCmd>());

    closeClient(cl.get(), sv.get());
}

TEST(ClientTests, ListRooms)
{
    auto t = startAndConnectClient();
    auto cl = std::get<0>(t);
    auto sv = std::get<1>(t);

    int roomsCount = 3;
    for (int i = 0; i < roomsCount; ++i)
    {
        std::stringstream ss;
        ss << "Room " << i;
        std::string roomName = ss.str();
        cl->executeCommand<commands::createRoomCmd>(roomName);

        EXPECT_TRUE(cl->executeCommand<commands::hasRoomCmd>());
        cl->executeCommand<commands::exitRoomCmd>();
        EXPECT_FALSE(cl->executeCommand<commands::hasRoomCmd>());
    }

    std::vector<netcode::room> rooms;
    cl->executeCommand<commands::getRoomsCmd>(rooms);

    for (const netcode::room& r : rooms)
    {
        std::cout << "Room Name: " << r.getName() << "\n";
    }

    EXPECT_EQ(roomsCount, rooms.size());

    closeClient(cl.get(), sv.get());
}

TEST(ClientTests, EnterRoom)
{
    auto t = startAndConnectClient();
    auto cl = std::get<0>(t);
    auto sv = std::get<1>(t);

    std::string roomName = "MyRoom";
    cl->executeCommand<commands::createRoomCmd>(roomName);
    int roomId = -1;

    EXPECT_TRUE(cl->executeCommand<commands::hasRoomCmd>());
    netcode::room* r;
    cl->executeCommand<commands::getRoomCmd>(r);
    roomId = r->getId();
    cl->executeCommand<commands::exitRoomCmd>();
    EXPECT_FALSE(cl->executeCommand<commands::hasRoomCmd>());

    cl->executeCommand<commands::enterRoomCmd>(roomId);

    EXPECT_TRUE(cl->executeCommand<commands::hasRoomCmd>());
    cl->executeCommand<commands::getRoomCmd>(r);
    EXPECT_EQ(roomId, r->getId());

    closeClient(cl.get(), sv.get());
}
