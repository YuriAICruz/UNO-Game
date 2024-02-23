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
            : element(pos, COORD{1, 1}, drawC, c), offset(offset)
        {
        }

        template <typename T, typename... Args>
        size_t addElement(Args... args)
        {
            elements.emplace_back(std::make_unique<T>(args...));
            resize();
            return elements.back()->getId();
        }

        element* getElement(size_t id) const;
        void removeElement(size_t id);
        virtual void resize() = 0;
    };
}
