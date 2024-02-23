#include "gtest/gtest.h"
#include "client/client.h"
#include "commands/client/enterRoomCmd.h"
#include "server/server.h"

TEST(clientsCmd, enterRoomCmd)
{
    auto sv = std::make_shared<netcode::server>();
    auto clA = std::make_shared<netcode::client>();
    auto clB = std::make_shared<netcode::client>();

    sv->start();
    clA->start();
    clA->connectToServer();
    clB->start();
    clB->connectToServer();

    auto roomName = "test";
    clA->createRoom(roomName);

    auto cmd = commands::enterRoomCmd(0, NC_ENTER_ROOM, clB.get());
    clB->executeCommand(&cmd);

    ASSERT_EQ(clB->getRoom()->getName(), roomName);
}
