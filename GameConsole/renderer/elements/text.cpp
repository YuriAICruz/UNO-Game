#include "text.h"

#include "../bufferData.h"

void elements::text::draw(std::vector<std::vector<renderer::bufferData>>* buffer) const
{    
    int xSize = buffer->at(0).size();
    int ySize = buffer->size();

    int length = textValue.length();

    for (int y = 0; y < ySize; ++y)
    {
        for (int x = 0; x < xSize; ++x)
        {
            if (y == position.Y &&
                x >= position.X && x < position.X + length)
            {
                buffer->at(y).at(x).c = textValue[x - position.X];
                buffer->at(y).at(x).color = color;
            }
        }
    }
}
