#pragma once
#include <string>

#include "element.h"

namespace elements
{
    class text : public element
    {
    private:
        std::string textValue;

    public:
        text(const COORD& pos, char drawC, const char& c, std::string text)
            : element(pos, COORD{static_cast<SHORT>(text.length()), 1}, drawC, c), textValue(text)
        {
            
        }
        void draw(std::vector<std::vector<renderer::bufferData>>* buffer) const override;
        void setText(const std::string& title);
    };
}
