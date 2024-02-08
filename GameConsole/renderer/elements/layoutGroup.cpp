#include "layoutGroup.h"

namespace elements
{
    size_t layoutGroup::addElement(std::unique_ptr<element> valuePtr)
    {
        elements.push_back(std::move(valuePtr));
        resize();
        return elements.back()->getId();
    }

    element* layoutGroup::getElement(size_t id) const
    {
        int index = 0;
        for (int n = elements.size(); index < n; ++index)
        {
            if (elements.at(index)->getId() == id)
            {
                break;
            }
        }

        if (index >= elements.size())
        {
            throw std::exception("no element found");
        }

        return elements.at(index).get();
    }

    void layoutGroup::removeElement(size_t id)
    {
        int index = 0;
        for (int n = elements.size(); index < n; ++index)
        {
            if (elements.at(index)->getId() == id)
            {
                break;
            }

            index++;
        }

        if (index >= elements.size())
        {
            throw std::exception("no element found");
        }
        elements.erase(elements.begin() + index);
        resize();
    }
}
