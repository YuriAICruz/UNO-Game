#pragma once
#include "element.h"
#include "square.h"

namespace elements
{
    class frame : public element
    {
    protected:
        square squareIn;
        wchar_t frameChars[6];

    public:
        frame(const COORD& pos, const COORD& size, const char drawC, const char& c):
            element(pos, size, drawC, c),
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
            setDefault();
        }


        void draw(std::vector<std::vector<renderer::bufferData>>* buffer) const override;
        void setDefault();
        void setFrame(wchar_t topLeft, wchar_t topRight, wchar_t bottomLeft, wchar_t bottomRight, wchar_t horizontal,
                      wchar_t vertical);
        void setPosition(COORD pos) override;
    };
}
