#pragma once
#include "actionType.h"
#include <typeinfo>

namespace cards
{
    namespace actions
    {
        class base : public actionType
        {
        public:
            bool isEqual(const type_info& actionType) override
            {
                return actionType.hash_code() == typeid(base).hash_code();
            }
    
        };
    }
}
