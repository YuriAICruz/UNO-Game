#pragma once
#include <vector>

#include "../guidGenerator.h"
#include "../pch.h"

namespace renderer
{
    struct bufferData;
}

namespace elements
{
    class element
    {
    protected:
        size_t id;
        COORD position;
        char drawChar;
        char color;

    public:
        element(const COORD& position, char drawChar, const char& color) : position(position), drawChar(drawChar), color(color)
        {
            id = guidGenerator::generateGUID();
        }

        virtual ~element() = default;
        virtual void draw(std::vector<std::vector<renderer::bufferData>>* buffer) const = 0;

        size_t getId()
        {
            return id;
        }
    };
}
