#include "gtest/gtest.h"
#include "server/server.h"

TEST(ServerTests, OpenAndClose)
{
    auto sv = std::make_unique<server>();

    sv->start();
    while (!sv->isRunning() && !sv->hasError())
    {
        
    }
    EXPECT_TRUE(sv->isRunning());
    sv->close();
    while (sv->isRunning())
    {
        
    }
    EXPECT_FALSE(sv->isRunning());
}
