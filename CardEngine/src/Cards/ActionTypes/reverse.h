#pragma once
#include "actionType.h"

namespace cards
{
    namespace actions
    {
        class reverse : public actionType
        {
        public:
            bool isEqual(const type_info& actionType) override
            {
                return actionType.hash_code() == typeid(reverse).hash_code(); 
            }
        };
    }
}
