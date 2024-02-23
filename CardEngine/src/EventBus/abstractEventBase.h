#pragma once
#include "../../framework.h"

namespace eventBus
{
    struct ENGINE_API abstractEventBase
    {
        virtual ~abstractEventBase() = default;
    };
}
