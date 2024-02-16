#pragma once
#include "../framework.h"

class ENGINE_API guidGenerator
{
private:
    static size_t counter;
public:
    static size_t generateGUID();
};
