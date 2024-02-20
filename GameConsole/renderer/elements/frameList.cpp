#include "frameList.h"

#include "../bufferData.h"

namespace elements
{
    void frameList::draw(std::vector<std::vector<renderer::bufferData>>* buffer) const
    {
        frameElement.draw(buffer);
        for (auto line : lines)
        {
            line.draw(buffer);
        }
    }

    void frameList::select()
    {
        frameElement.setFrame(L'╔',
                              L'╖',
                              L'╚',
                              L'╛',
                              L'═',
                              L'║');
    }

    void frameList::deselect()
    {
        frameElement.setDefault();
    }

    void frameList::setPosition(COORD pos)
    {
        element::setPosition(pos);
        frameElement.setPosition(pos);
        repositionLines();
    }

    void frameList::setSize(COORD s)
    {
        element::setSize(s);
        frameElement.setSize(s);
        repositionLines();

        setPosition(position);
    }

    void frameList::setColor(char c)
    {
        element::setColor(c);
        frameElement.setColor(c);
        for (auto line : lines)
        {
            line.setColor(c);
        }
    }

    void frameList::addText(std::string newText)
    {
        lines.push_back(text{
            COORD{
                static_cast<SHORT>(0),
                static_cast<SHORT>(0)
            },
            ' ',
            color,
            newText
        });
        repositionLines();
    }

    void frameList::setText(int index, std::string newText)
    {
        lines[index].setText(newText);
        repositionLines();
    }

    void frameList::repositionLines()
    {
        constexpr int border = 1;
        int lastX = frameElement.getPosition().X + border;
        int lastY = frameElement.getPosition().Y + border;

        for (auto& line : lines)
        {
            constexpr int lineSize = 1;
            line.setPosition(COORD{
                static_cast<SHORT>(lastX),
                static_cast<SHORT>(lastY)
            });
            lastY += lineSize;
        }
    }

    int frameList::linesCount() const
    {
        return lines.size();
    }
}
