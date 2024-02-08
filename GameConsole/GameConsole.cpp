#include <memory>

#include "Bootstrapper/bootstrapper.h"
#include "input/inputHandler.h"
#include "renderer/renderer.h"
#include "screens/mainMenuScreen.h"
#include "eventIds.h"

int main()
{
    std::unique_ptr<bootstrapper> strapper = std::make_unique<bootstrapper>();
    strapper->bind<eventBus::eventBus>()->to<eventBus::eventBus>()->asSingleton();
    strapper->bind<renderer::renderer>()->to<renderer::renderer>()->asSingleton();

    auto rdr = strapper->create<renderer::renderer>();
    auto events = strapper->create<eventBus::eventBus>();

    events->bindEvent<input::inputData>(INPUT_UP);
    events->bindEvent<input::inputData>(INPUT_DOWN);
    events->bindEvent<input::inputData>(INPUT_LEFT);
    events->bindEvent<input::inputData>(INPUT_RIGHT);

    auto inputH = std::make_unique<input::inputHandler>(strapper->create<eventBus::eventBus>());

    std::unique_ptr<screens::IScreen> mainMenu = std::make_unique<screens::mainMenuScreen>(
        strapper->create<renderer::renderer>(),
        strapper->create<eventBus::eventBus>()
    );
    mainMenu->show();

    while (true)
    {
        std::vector<input::inputData> inputs = inputH->readInput();
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
