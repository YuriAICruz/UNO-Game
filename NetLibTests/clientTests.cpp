﻿#include "client/client.h"
#include "gtest/gtest.h"
#include "server/server.h"
#include <tuple>

#include "logger.h"

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
}

TEST(ClientTests, Close)
{
    auto cl = std::make_unique<client>();

    cl->start();
    EXPECT_TRUE(cl->isRunning());

    cl->close();
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
    closeClient(cl.get(), sv.get());
}

TEST(ClientTests, ExitRoom)
{
    auto t = startAndConnectClient();
    auto cl = std::get<0>(t);
    auto sv = std::get<1>(t);
    closeClient(cl.get(), sv.get());
}

TEST(ClientTests, EnterRoom)
{
    auto t = startAndConnectClient();
    auto cl = std::get<0>(t);
    auto sv = std::get<1>(t);
    closeClient(cl.get(), sv.get());
}
