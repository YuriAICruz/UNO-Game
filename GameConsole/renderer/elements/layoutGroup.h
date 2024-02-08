#pragma once
#include <memory>
#include <vector>

#include "element.h"

namespace elements
{
    class layoutGroup : public element
    {
    protected:
        std::vector<std::unique_ptr<elements::element>> elements;
        int offset;

    public:
        layoutGroup(const COORD& pos, char drawC, const char& c, int offset)
            : element(pos, COORD{1,1}, drawC, c), offset(offset)
        {
        }

        size_t addElement(std::unique_ptr<element> valuePtr);
        element* getElement(size_t id) const;
        void removeElement(size_t id);
        virtual void resize() = 0;
    };
}
