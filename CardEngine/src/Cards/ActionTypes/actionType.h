#pragma once
#include "../../framework.h"

namespace cards
{
    namespace actions
    {
        class ENGINE_API actionType
        {
        public:
            virtual ~actionType() = default;
            virtual bool isEqual(const type_info& actionType) = 0;
        };
    }
}