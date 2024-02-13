#include "client/client.h"
#include "gtest/gtest.h"

TEST(ClientTests, Create)
{
    auto cl = std::make_unique<client>();
    cl->start();

    EXPECT_TRUE(cl->isRunning());
}
