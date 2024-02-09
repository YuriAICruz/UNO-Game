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

    void card::select()
    {
        frameElement.setFrame(L'╔',
                              L'╖',
                              L'╚',
                              L'╛',
                              L'═',
                              L'║');
    }

    void card::deselect()
    {
        frameElement.setDefault();
    }

    void card::setPosition(COORD pos)
    {
        element::setPosition(pos);
        frameElement.setPosition(pos);
        textElement.setPosition(
                COORD{
                    static_cast<SHORT>(pos.X + (size.X / 2) - title.length() / 2),
                    static_cast<SHORT>(pos.Y + 1)
                });
        centerTextElement.setPosition(
                COORD{
                    static_cast<SHORT>(pos.X + (size.X / 2) - centerText.length() / 2),
                    static_cast<SHORT>(pos.Y + (size.Y / 2))
                });
    }

    void card::setTitleText(std::string newText)
    {
        title = newText;
        textElement.setText(title);
        textElement.setPosition(
                COORD{
                    static_cast<SHORT>(position.X + (size.X / 2) - title.length() / 2),
                    static_cast<SHORT>(position.Y + 1)
                });
    }

    void card::setCenterText(std::string newText)
    {
        centerText = newText;
        centerTextElement.setText(centerText);
        centerTextElement.setPosition(
                COORD{
                    static_cast<SHORT>(position.X + (size.X / 2) - centerText.length() / 2),
                    static_cast<SHORT>(position.Y + (size.Y / 2))
                });
    }
}
