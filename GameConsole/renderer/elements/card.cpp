#include "card.h"

#include "../bufferData.h"

namespace elements
{
    void card::draw(std::vector<std::vector<renderer::bufferData>>* buffer) const
    {
        frameElement.draw(buffer);
        textElement.draw(buffer);
        centerTextElement.draw(buffer);
    }
}
