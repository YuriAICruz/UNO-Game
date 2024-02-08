#include "card.h"

namespace elements
{
    void frame::draw(std::vector<std::vector<char>>* buffer) const
    {
        squareOut.draw(buffer);
        squareIn.draw(buffer);
    }
}
