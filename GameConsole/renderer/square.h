#pragma once
#include "element.h"

class square : public element
{
private:
    COORD size;

public:
    square(COORD position, COORD size): element(position), size(size)
    {
    }

    void draw(std::vector<std::vector<char>>* buffer) override;
};
