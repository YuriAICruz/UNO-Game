#pragma once
#include <vector>

#include "guidGenerator.h"
#include "../pch.h"

class element
{
protected:
    size_t id;
    COORD position;

public:
    element(COORD position) : position(position)
    {
        id = guidGenerator::generateGUID();
    }

    virtual ~element() = default;
    virtual void draw(std::vector<std::vector<char>>* buffer) = 0;

    size_t getId()
    {
        return id;
    }
};
