#include "card.h"

#include "../bufferData.h"

namespace elements
{
    void frame::setFrame(wchar_t topLeft, wchar_t topRight, wchar_t bottomLeft, wchar_t bottomRight, wchar_t horizontal,
                         wchar_t vertical)
    {
        frameChars[0] = topLeft;
        frameChars[1] = topRight;
        frameChars[2] = bottomLeft;
        frameChars[3] = bottomRight;
        frameChars[4] = horizontal;
        frameChars[5] = vertical;
    }

    void frame::setPosition(COORD pos)
    {
        element::setPosition(pos);

        squareIn.setPosition(
            COORD{
                static_cast<SHORT>(pos.X + 1),
                static_cast<SHORT>(pos.Y + 1)
            });
    }

    void frame::setDefault()
    {
        setFrame(L'┌',
                 L'┐',
                 L'└',
                 L'┘',
                 L'─',
                 L'│');
    }

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
                            buffer->at(y).at(x).c = frameChars[0]; // ┌
                        }
                        else if (x == position.X + size.X - 1)
                        {
                            buffer->at(y).at(x).c = frameChars[1]; // ┐
                        }
                        else
                        {
                            buffer->at(y).at(x).c = frameChars[4]; // ─                            
                        }
                    }
                    else if (y == position.Y + size.Y - 1)
                    {
                        if (x == position.X)
                        {
                            buffer->at(y).at(x).c = frameChars[2]; // └
                        }
                        else if (x == position.X + size.X - 1)
                        {
                            buffer->at(y).at(x).c = frameChars[3]; // ┘
                        }
                        else
                        {
                            buffer->at(y).at(x).c = frameChars[4]; // ─                            
                        }
                    }
                    else
                    {
                        buffer->at(y).at(x).c = frameChars[5]; // │
                    }
                    buffer->at(y).at(x).color = color;
                }
            }
        }
        squareIn.draw(buffer);
    }
}
