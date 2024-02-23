#include "fileRead.h"
#include "../bufferData.h"

namespace elements
{
    void fileRead::draw(std::vector<std::vector<renderer::bufferData>>* buffer) const
    {
        int xSize = buffer->at(0).size();
        int ySize = buffer->size();

        for (int y = 0; y < ySize; ++y)
        {
            for (int x = 0; x < xSize; ++x)
            {
                int localX = x - position.X;
                int localY = y - position.Y;
                if (y >= position.Y && y < position.Y + fileData.size() &&
                    localY < fileData.size() &&
                    localX < fileData.at(localY).size() &&
                    x >= position.X && x < position.X + fileData.at(localY).size())
                {
                    buffer->at(y).at(x).c = fileData.at(localY).at(localX);
                    buffer->at(y).at(x).color = color;
                }
            }
        }
    }
}
