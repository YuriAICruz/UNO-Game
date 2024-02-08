#pragma once
#include "element.h"

namespace elements
{
    class square : public element
    {
    private:
        COORD size;

    public:
        square(const COORD& position, const COORD& size, const char drawChar, const char& color): element(position, drawChar, color), size(size)
        {
        }

        void draw(std::vector<std::vector<renderer::bufferData>>* buffer) const override;
    };
}
