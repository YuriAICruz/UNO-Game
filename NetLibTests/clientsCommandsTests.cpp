#include "gtest/gtest.h"
#include "client/client.h"
#include "commands/client/createRoomCmd.h"
#include "commands/client/enterRoomCmd.h"
#include "server/server.h"

TEST(clientsCmd, enterRoomCmd)
{
    auto sv = std::make_unique<netcode::server>();
    auto clA = std::make_unique<netcode::client>();
    auto clB = std::make_unique<netcode::client>();

    sv->start();
    clA->start();
    clA->connectToServer();
    clB->start();
    clB->connectToServer();

    std::string roomName = "test";
    clA->executeCommand<commands::createRoomCmd>(roomName);
    clB->executeCommand<commands::enterRoomCmd>(0);

    netcode::room* r;
    clB->executeCommand<commands::getRoomCmd>(r);
    ASSERT_EQ(r->getName(), roomName);
}
