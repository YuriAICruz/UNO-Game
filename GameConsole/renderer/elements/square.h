#pragma once
#include "element.h"

namespace elements
{
    class square : public element
    {
    public:
        square(const COORD& position, const COORD& size, const char drawChar, const char& color): element(position, size, drawChar, color)
        {
        }

        void draw(std::vector<std::vector<renderer::bufferData>>* buffer) const override;
    };
}
