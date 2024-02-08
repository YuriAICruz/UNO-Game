#include <memory>
#include "renderer/renderer.h"
#include "renderer/elements/card.h"
#include "renderer/elements/frame.h"
#include "renderer/elements/horizontalLayoutGroup.h"
#include "renderer/elements/square.h"
#include "renderer/elements/text.h"

int main()
{
    auto rdr = std::make_unique<renderer::renderer>();

    rdr->addElement<elements::frame>(COORD{10, 10}, COORD{5, 10}, '*', 'w');
    rdr->addElement<elements::text>(COORD{8, 19}, '*', 'r', "/\\square/\\");
    rdr->addElement<elements::card>(COORD{40, 5}, COORD{6, 8}, '+', 'g', "draw", "+2");
    auto cardId = rdr->addElement<elements::card>(COORD{34, 5}, COORD{6, 8}, '+', 'g', "draw", "+2");
    auto card = rdr->getElement(cardId);
    static_cast<elements::card*>(card)->select();
    rdr->addElement<elements::square>(COORD{15, 0}, COORD{10, 10}, '.', 'b');
    rdr->addElement<elements::square>(COORD{0, 20}, COORD{1000, 1}, '_', 'y');
    rdr->addElement<elements::square>(COORD{0, 21}, COORD{1000, 1}, '<', 'o');
    rdr->addElement<elements::square>(COORD{0, 22}, COORD{1000, 1}, '>', 'p');
    rdr->addElement<elements::square>(COORD{0, 23}, COORD{1000, 1}, '┌', 'c');
    
    auto lGroupId = rdr->addElement<elements::horizontalLayoutGroup>(COORD{10, 1}, '+', 'g', 2);
    auto lGroup = static_cast<elements::horizontalLayoutGroup*>(rdr->getElement(lGroupId));
    SHORT sizeX = 6;
    SHORT sizeY = 5;
    lGroup->addElement<elements::card>(COORD{0, 0}, COORD{sizeX, sizeY}, '+', 'g', "draw", "+2");
    lGroup->addElement<elements::card>(COORD{0, 0}, COORD{sizeX, sizeY}, '+', 'r', "UNO", "1");
    lGroup->addElement<elements::card>(COORD{0, 0}, COORD{sizeX, sizeY}, '+', 'g', "UNO", "3");
    lGroup->addElement<elements::card>(COORD{0, 0}, COORD{sizeX, sizeY}, '+', 'y', "UNO", "9");
    lGroup->addElement<elements::card>(COORD{0, 0}, COORD{sizeX, sizeY}, '+', 'b', "UNO", "4");
    lGroup->addElement<elements::card>(COORD{0, 0}, COORD{sizeX, sizeY}, '+', 'y', "UNO", "8");
    lGroup->addElement<elements::card>(COORD{0, 0}, COORD{sizeX, sizeY}, '+', 'r', "UNO", "7");

    while (true)
    {
        if (rdr->isDirty())
        {
            rdr->draw();
        }

        if (rdr->canEnd())
        {
            break;
        }
    }
    return 0;
}
