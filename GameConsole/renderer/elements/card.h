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
        frame frameElement;
        text textElement;
        text centerTextElement;
        std::string centerText;
        std::string title;

    public:
        card(const COORD& pos, const COORD& size, char drawC, const char& c, std::string title,
             std::string centerText) :
            element(pos, size, drawC, c),
            frameElement(pos, size, drawC, c),
            title(title),
            textElement(
                COORD{
                    static_cast<SHORT>(position.X + (size.X / 2) - title.length() / 2),
                    static_cast<SHORT>(position.Y + 1)
                },
                drawC,
                c,
                title
            ),
            centerText(centerText),
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

        void select();
        void deselect();
        void setPosition(COORD pos) override;
        void setSize(COORD s) override;
        void setTitleText(std::string newText);
        void setCenterText(std::string newText);

    private:
    };
}
