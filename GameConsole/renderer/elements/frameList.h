#pragma once
#include <string>

#include "element.h"
#include "frame.h"
#include "text.h"
#include "Cards/ICard.h"

namespace elements
{
    class frameList : public element
    {
    private:
        frame frameElement;
        std::vector<text> lines{};

    public:
        frameList(const COORD& pos, const COORD& size, char drawC, const char& c) :
            element(pos, size, drawC, c),
            frameElement(pos, size, drawC, c)
        {
        }

        void draw(std::vector<std::vector<renderer::bufferData>>* buffer) const override;

        void select();
        void deselect();
        void setPosition(COORD pos) override;
        void setSize(COORD s) override;
        void setColor(char c) override;
        void addText(std::string newText);
        void setText(int index, std::string newText);
        void repositionLines();
        int linesCount() const;

    private:
    };
}
