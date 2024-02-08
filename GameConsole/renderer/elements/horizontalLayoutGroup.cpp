#include "horizontalLayoutGroup.h"

namespace elements
{
    void horizontalLayoutGroup::draw(std::vector<std::vector<renderer::bufferData>>* buffer) const
    {
        for (int i = 0, n = elements.size(); i < n; ++i)
        {
            elements.at(i)->draw(buffer);
        }
    }

    void horizontalLayoutGroup::resize()
    {
        int length = 0;
        for (int i = 0, n = elements.size(); i < n; ++i)
        {
            elements.at(i)->setPosition(
                COORD
                {
                    static_cast<SHORT>(position.X + length),
                    position.Y
                }
            );
            length += elements.at(i)->getSize().X + offset;
        }
    }
}
