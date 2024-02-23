#pragma once
#include "../../framework.h"
#include <tuple>

class ENGINE_API IStateManager
{
public:
    virtual ~IStateManager() = default;
    virtual std::tuple<const char*, size_t> getState() = 0;
    virtual void setState(const char* data, size_t size) = 0;
};
