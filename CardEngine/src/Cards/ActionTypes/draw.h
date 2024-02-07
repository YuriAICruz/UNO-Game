#pragma once
#include "actionType.h"
#include <typeinfo>

namespace cards
{
    namespace actions
    {
        class draw : public actionType
        {
        public:
            bool isEqual(const type_info& actionType) override
            {
                return actionType.hash_code() == typeid(draw).hash_code();
            }
    
        };
    }
}
