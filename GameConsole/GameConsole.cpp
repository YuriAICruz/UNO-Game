#include <memory>
#include "renderer/renderer.h"
#include "renderer/square.h"

int main()
{
    auto rdr = std::make_unique<renderer::renderer>();

    rdr->addElement(std::make_unique<square>(COORD{10, 10}, COORD{5, 10}));
    rdr->addElement(std::make_unique<square>(COORD{15, 0}, COORD{10, 10}));

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
