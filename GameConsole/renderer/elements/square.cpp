#include "square.h"

#include "../bufferData.h"

namespace elements
{
    void square::draw(std::vector<std::vector<renderer::bufferData>>* buffer) const
    {
        int xSize = buffer->at(0).size();
        int ySize = buffer->size();

        for (int y = 0; y < ySize; ++y)
        {
            for (int x = 0; x < xSize; ++x)
            {
                if (y >= position.Y && y < position.Y + size.Y &&
                    x >= position.X && x < position.X + size.X)
                {
                    buffer->at(y).at(x).c = drawChar;
                    buffer->at(y).at(x).color = color;
                }
            }
        }
    }
}
