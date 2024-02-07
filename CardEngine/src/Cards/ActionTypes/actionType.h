#pragma once

namespace cards
{
    namespace actions
    {
        class actionType
        {
        public:
            virtual ~actionType() = default;
            virtual bool isEqual(const type_info& actionType) = 0;
        };
    }
}