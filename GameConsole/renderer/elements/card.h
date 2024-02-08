#pragma once
#include <string>

#include "element.h"
#include "frame.h"
#include "text.h"

namespace elements
{
    class card : public element
    {
    private:
        const std::string title;
        const frame frameElement;
        const text textElement;
        const COORD size;

    public:
        card(const COORD& pos, const COORD& size, char drawC, const char& c, std::string title) :
            element(pos, drawC, c),
            title(title),
            size(size),
            frameElement(pos, size, drawC, c),
            textElement(
                COORD{
                    static_cast<SHORT>(position.X + 1),
                    static_cast<SHORT>(position.Y + 1)
                },
                drawC,
                c,
                title
            )
        {
        }

        void draw(std::vector<std::vector<char>>* buffer) const override
        {
            frameElement.draw(buffer);
            textElement.draw(buffer);
        }

    private:
    };
}
