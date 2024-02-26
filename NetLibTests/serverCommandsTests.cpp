#include "gtest/gtest.h"
#include "client/client.h"
#include "commands/client/createRoomCmd.h"
#include "commands/client/enterRoomCmd.h"
#include "commands/client/exitRoomCmd.h"
#include "commands/client/getRoomCmd.h"
#include "commands/server/enterRoomServerCmd.h"
#include "server/server.h"

TEST(ServerCmd, enterRoomCmd)
{
    auto sv = std::make_unique<netcode::server>();
    auto cl = std::make_unique<netcode::client>();

    sv->start();
    cl->start();
    cl->connectToServer();
    
    sv->executeServerCommand<commands::enterRoomServerCmd>();

    cl->executeCommand<commands::createRoomCmd>("Room");

    netcode::room* r;
    cl->executeCommand<commands::getRoomCmd>(r);
    uint16_t id = r->getId();

    cl->executeCommand<commands::exitRoomCmd>();

    cl->executeCommand<commands::enterRoomCmd>(id);

    cl->close();
    sv->close();
}
