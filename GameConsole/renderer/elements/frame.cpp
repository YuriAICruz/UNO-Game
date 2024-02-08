#include "card.h"

#include "../bufferData.h"

namespace elements
{
    void frame::draw(std::vector<std::vector<renderer::bufferData>>* buffer) const
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
                    if (y == position.Y)
                    {
                        if (x == position.X)
                        {
                            buffer->at(y).at(x).c = L'┌'; // ┌
                        }
                        else if (x == position.X + size.X - 1)
                        {
                            buffer->at(y).at(x).c = L'┐'; // ┐
                        }
                        else
                        {
                            buffer->at(y).at(x).c = L'─'; // ─                            
                        }
                    }
                    else if (y == position.Y + size.Y - 1)
                    {
                        if (x == position.X)
                        {
                            buffer->at(y).at(x).c = L'└'; // └
                        }
                        else if (x == position.X + size.X - 1)
                        {
                            buffer->at(y).at(x).c = L'┘'; // ┘
                        }
                        else
                        {
                            buffer->at(y).at(x).c = L'─'; // ─                            
                        }
                    }
                    else
                    {
                        buffer->at(y).at(x).c = L'│'; // │
                    }
                    buffer->at(y).at(x).color = color;
                }
            }
        }
        //squareOut.draw(buffer);
        squareIn.draw(buffer);
    }
}
