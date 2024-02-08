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
        const COORD size;
        const frame frameElement;
        const text textElement;
        const text centerTextElement;

    public:
        card(const COORD& pos, const COORD& size, char drawC, const char& c, std::string title,
             std::string centerText) :
            element(pos, drawC, c),
            size(size),
            frameElement(pos, size, drawC, c),
            textElement(
                COORD{
                    static_cast<SHORT>(position.X + (size.X / 2) - title.length() / 2),
                    static_cast<SHORT>(position.Y + 1)
                },
                drawC,
                c,
                title
            ),
            centerTextElement(
                COORD{
                    static_cast<SHORT>(position.X + (size.X / 2) - centerText.length() / 2),
                    static_cast<SHORT>(position.Y + (size.Y / 2))
                },
                drawC,
                c,
                centerText
            )
        {
        }

        void draw(std::vector<std::vector<renderer::bufferData>>* buffer) const override;

    private:
    };
}
