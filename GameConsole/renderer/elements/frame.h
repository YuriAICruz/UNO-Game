#pragma once
#include "element.h"
#include "square.h"

namespace elements
{
    class frame : public element
    {
    protected:
        COORD size;
        square squareOut;
        square squareIn;

    public:
        frame(const COORD& pos, const COORD& size, const char drawC, const char& c):
            element(pos, drawC, c),
            size(size),
            squareOut(pos, size, drawC, c),
            squareIn(
                COORD{
                    static_cast<SHORT>(pos.X + 1),
                    static_cast<SHORT>(pos.Y + 1)
                },
                COORD{
                    static_cast<SHORT>(size.X - 2),
                    static_cast<SHORT>(size.Y - 2)
                },
                ' ',
                c
            )
        {
        }

        void draw(std::vector<std::vector<renderer::bufferData>>* buffer) const override;
    };
}
