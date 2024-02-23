#include "pch.h"

#include "EventBus/eventBus.h"

int intData;

void intFoo(const int i)
{
    intData = i;
}

std::string stringData;

void stringFoo(const std::string& data)
{
    stringData = data;
}

TEST(EventBus, Bind)
{
    auto eventBus = std::make_unique<eventBus::eventBus>();
    eventBus->bindEvent<int>(0);
    eventBus->bindEvent<std::string>(2);
}

TEST(EventBus, Subscribe)
{
    auto eventBus = std::make_unique<eventBus::eventBus>();
    eventBus->bindEvent<int>(0);
    eventBus->bindEvent<std::string>(2);

    eventBus->subscribe<int>(0, intFoo);
    eventBus->subscribe<std::string>(2, stringFoo);
}
TEST(EventBus, SubscribeAndFire)
{
     auto eventBus = std::make_unique<eventBus::eventBus>();
     eventBus->bindEvent<int>(0);
     eventBus->bindEvent<const std::string&>(2);

     eventBus->subscribe<int>(0, intFoo);
     eventBus->subscribe<const std::string&>(2, stringFoo);

     eventBus->fireEvent(0, 123);
     eventBus->fireEvent(2, std::string("HelloWorld"));

    EXPECT_EQ(123, intData);
    EXPECT_EQ("HelloWorld", stringData);
}
