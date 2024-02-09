#pragma once
#include "actionType.h"

namespace cards
{
    namespace actions
    {
        class skip : public actionType
        {
        public:
            bool isEqual(const type_info& actionType) override
            {
                return actionType.hash_code() == typeid(skip).hash_code();
            }
    
        };
    }
}
