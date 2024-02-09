#pragma once
#include <vector>

#include "guidGenerator.h"
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
        const size_t id;
        COORD position;
        COORD size;
        char drawChar;
        char color;

    public:
        element(const COORD& position, COORD size, char drawChar, const char& color) :
            position(position),
            size(size),
            drawChar(drawChar),
            color(color),
            id(guidGenerator::generateGUID())
        {
        }

        virtual ~element() = default;
        virtual void draw(std::vector<std::vector<renderer::bufferData>>* buffer) const = 0;

        size_t getId()
        {
            return id;
        }

        virtual void setPosition(COORD pos)
        {
            this->position = pos;
        }

        COORD getSize()
        {
            return size;
        }

        virtual void setSize(COORD s)
        {
            this->size = s;
        }

        virtual void setColor(char c)
        {
            color = c;
        }
    };
}
