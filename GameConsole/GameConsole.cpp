#include <memory>

#ifndef PCH_H
#define PCH_H
#include "wInclude.h"
#endif //PCH_H

#include "Bootstrapper/bootstrapper.h"
#include "input/inputHandler.h"
#include "renderer/renderer.h"
#include "screens/mainMenuScreen.h"
#include "eventIds.h"
#include "netGameStateManager.h"
#include "renderer/elements/fileRead.h"
#include "screens/connectToServerScreen.h"
#include "screens/gameScreen.h"
#include "screens/settingsMenuScreen.h"
#include "screens/gameOverScreen.h"
#include "StateManager/gameStateManager.h"
#include "client/client.h"
#include "screens/roomCreationScreen.h"
#include "screens/roomWaitingScreen.h"

int main()
{
    std::unique_ptr<bootstrapper> strapper = std::make_unique<bootstrapper>();
    strapper->bind<eventBus::eventBus>()->to<eventBus::eventBus>()->asSingleton();
    strapper->bind<renderer::renderer>()->to<renderer::renderer>()->asSingleton();
    strapper->bind<gameStateManager, std::shared_ptr<eventBus::eventBus>>()->to<gameStateManager>()->asSingleton();
    strapper->bind<netGameStateManager, std::shared_ptr<eventBus::eventBus>, std::shared_ptr<netcode::client>>()->to<netGameStateManager>()->asSingleton();
    strapper->bind<netcode::client>()->to<netcode::client>()->asSingleton();

    auto gameManager = strapper->create<gameStateManager>(strapper->create<eventBus::eventBus>());
    auto netGameManager = strapper->create<netGameStateManager>(strapper->create<eventBus::eventBus>(), strapper->create<netcode::client>());
    auto rdr = strapper->create<renderer::renderer>();
    auto events = strapper->create<eventBus::eventBus>();
    auto netClient = strapper->create<netcode::client>();

    gameManager->bindGameEvents();
    events->bindEvent<input::inputData>(INPUT_UP);
    events->bindEvent<input::inputData>(INPUT_DOWN);
    events->bindEvent<input::inputData>(INPUT_LEFT);
    events->bindEvent<input::inputData>(INPUT_RIGHT);
    events->bindEvent<input::inputData>(INPUT_OK);
    events->bindEvent<input::inputData>(INPUT_CANCEL);
    events->bindEvent<input::inputData>(NAVIGATION_MAIN_MENU);
    events->bindEvent<input::inputData>(NAVIGATION_GAME);
    events->bindEvent<input::inputData>(NAVIGATION_ONLINE_GAME);
    events->bindEvent<input::inputData>(NAVIGATION_SETTINGS);
    events->bindEvent<input::inputData>(NAVIGATION_GAME_OVER);
    events->bindEvent<input::inputData>(NAVIGATION_NETWORK_CONNECT);
    events->bindEvent<input::inputData>(NAVIGATION_NETWORK_ROOMS);
    events->bindEvent<input::inputData>(NAVIGATION_NETWORK_WAIT_ROOM);

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
        strapper->create<eventBus::eventBus>()
    );
    std::shared_ptr<screens::gameOverScreen> gameOver = std::make_shared<screens::gameOverScreen>(
        strapper->create<renderer::renderer>(),
        strapper->create<eventBus::eventBus>()
    );
    std::shared_ptr<screens::connectToServerScreen> connectToServer = std::make_shared<screens::connectToServerScreen>(
        strapper->create<renderer::renderer>(),
        strapper->create<eventBus::eventBus>(),
        strapper->create<netcode::client>()
    );
    std::shared_ptr<screens::roomCreationScreen> roomCreation = std::make_shared<screens::roomCreationScreen>(
        strapper->create<renderer::renderer>(),
        strapper->create<eventBus::eventBus>(),
        strapper->create<netcode::client>()
    );
    std::shared_ptr<screens::roomWaitingScreen> roomWaiting = std::make_shared<screens::roomWaitingScreen>(
        strapper->create<renderer::renderer>(),
        strapper->create<eventBus::eventBus>(),
        strapper->create<netcode::client>()
    );

    events->subscribe<screens::transitionData>(
        NAVIGATION_MAIN_MENU, [settingsMenu, gameManager, netGameManager](screens::transitionData data)
        {
            gameManager->setupGame(
                settingsMenu->getPlayers(),
                settingsMenu->getHandCount(),
                settingsMenu->getConfigFilePath(),
                settingsMenu->getSeed()
            );
        });
    events->subscribe<screens::transitionData>(
        NAVIGATION_GAME, [game, settingsMenu, gameManager](screens::transitionData data)
        {
            game->setGameManager(gameManager.get());
            game->showWarnings(settingsMenu->canShowWarnings());
            game->show();
            gameManager->startGame();
        });
    events->subscribe<screens::transitionData>(
        NAVIGATION_NETWORK_WAIT_ROOM, [roomWaiting, settingsMenu, netGameManager](screens::transitionData data)
        {
            roomWaiting->setGameManager(netGameManager.get());
            roomWaiting->setGameSettings(settingsMenu->getConfigFilePath(), settingsMenu->getSeed());
            roomWaiting->show();
        });
    events->subscribe<screens::transitionData>(
        NAVIGATION_ONLINE_GAME, [game, settingsMenu, netGameManager](screens::transitionData data)
        {
            game->setGameManager(netGameManager.get());
            game->showWarnings(false);
            game->show();
            if(!netGameManager->isGameRunning())
            {
                netGameManager->startGame();
            }
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
                }
                rdr->blank();
                rdr->forceRedraw();
                rdr->setDirty();
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
