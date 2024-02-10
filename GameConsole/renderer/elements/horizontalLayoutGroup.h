#pragma once
#include "layoutGroup.h"

namespace elements
{
    class horizontalLayoutGroup : public layoutGroup
    {
    public:
        horizontalLayoutGroup(const COORD& pos, char drawC, const char& c, int offset)
            : layoutGroup(pos, drawC, c, offset)
        {
        }

        void draw(std::vector<std::vector<renderer::bufferData>>* buffer) const override;
        void resize() override;
    };
}
