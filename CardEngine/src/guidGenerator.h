#pragma once

class guidGenerator
{
private:
    static size_t counter;
public:
    static size_t generateGUID();
};
