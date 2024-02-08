#include <memory>
#include "renderer/renderer.h"
#include "renderer/elements/card.h"
#include "renderer/elements/frame.h"
#include "renderer/elements/square.h"
#include "renderer/elements/text.h"

int main()
{
    auto rdr = std::make_unique<renderer::renderer>();

    rdr->addElement(std::make_unique<elements::frame>(COORD{10, 10}, COORD{5, 10}, '*', 'w'));
    rdr->addElement(std::make_unique<elements::text>(COORD{8, 19}, '*', 'w', "/\\square/\\"));
    rdr->addElement(std::make_unique<elements::card>(COORD{40, 5}, COORD{10, 15}, '+', 'r', "reverse card"));
    rdr->addElement(std::make_unique<elements::square>(COORD{15, 0}, COORD{10, 10}, '.', 'r'));
    rdr->addElement(std::make_unique<elements::square>(COORD{0, 20}, COORD{1000, 1}, '_', 'b'));
    rdr->addElement(std::make_unique<elements::square>(COORD{0, 21}, COORD{1000, 1}, '<', 'y'));

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
