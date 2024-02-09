#include <memory>

#include "Bootstrapper/bootstrapper.h"
#include "input/inputHandler.h"
#include "renderer/renderer.h"
#include "screens/mainMenuScreen.h"
#include "eventIds.h"
#include "screens/gameScreen.h"
#include "screens/settingsMenuScreen.h"
#include "StateManager/gameStateManager.h"

int main()
{
    std::unique_ptr<bootstrapper> strapper = std::make_unique<bootstrapper>();
    strapper->bind<eventBus::eventBus>()->to<eventBus::eventBus>()->asSingleton();
    strapper->bind<renderer::renderer>()->to<renderer::renderer>()->asSingleton();
    strapper->bind<gameStateManager, std::shared_ptr<eventBus::eventBus>>()->to<gameStateManager>()->asSingleton();

    auto gameManager = strapper->create<gameStateManager>(strapper->create<eventBus::eventBus>());
    auto rdr = strapper->create<renderer::renderer>();
    auto events = strapper->create<eventBus::eventBus>();

    events->bindEvent<input::inputData>(INPUT_UP);
    events->bindEvent<input::inputData>(INPUT_DOWN);
    events->bindEvent<input::inputData>(INPUT_LEFT);
    events->bindEvent<input::inputData>(INPUT_RIGHT);
    events->bindEvent<input::inputData>(INPUT_OK);
    events->bindEvent<input::inputData>(INPUT_CANCEL);
    events->bindEvent<input::inputData>(NAVIGATION_MAIN_MENU);
    events->bindEvent<input::inputData>(NAVIGATION_GAME);
    events->bindEvent<input::inputData>(NAVIGATION_SETTINGS);
    events->bindEvent<input::inputData>(NAVIGATION_GAME_OVER);

    auto inputH = std::make_unique<input::inputHandler>(strapper->create<eventBus::eventBus>());

    std::unique_ptr<screens::IScreen> mainMenu = std::make_unique<screens::mainMenuScreen>(
        strapper->create<renderer::renderer>(),
        strapper->create<eventBus::eventBus>()
    );
    std::shared_ptr<screens::settingsMenuScreen> settingsMenu = std::make_shared<screens::settingsMenuScreen>(
        strapper->create<renderer::renderer>(),
        strapper->create<eventBus::eventBus>()
    );
    std::shared_ptr<screens::gameScreen> game = std::make_shared<screens::gameScreen>(
        strapper->create<renderer::renderer>(),
        strapper->create<eventBus::eventBus>(),
        strapper->create<gameStateManager>()
    );

    events->subscribe<screens::transitionData>(
        NAVIGATION_MAIN_MENU, [settingsMenu, gameManager](screens::transitionData data)
        {
            gameManager->setupGame(
                settingsMenu->getPlayers(),
                settingsMenu->getHandCount(),
                settingsMenu->getConfigFilePath(),
                settingsMenu->getSeed()
            );
        });
    events->subscribe<screens::transitionData>(
        NAVIGATION_GAME, [settingsMenu, gameManager](screens::transitionData data)
        {
            gameManager->startGame();
        });

    events->fireEvent(NAVIGATION_MAIN_MENU, screens::transitionData());

    while (true)
    {
        std::vector<input::inputData> inputs = inputH->readInput();
        for (input::inputData input : inputs)
        {
            if (input.character == '\'')
            {
                rdr->blank();
                std::cout << "Cheat Console:\n";
                std::string input;
                std::cin >> input;
                if (input == "win")
                {
                    gameManager->cheatWin();
                    rdr->blank();
                    rdr->isDirty();
                }
            }
        }
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
