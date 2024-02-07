#include <iostream>

#include "Cards/ICard.h"

int main()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD bufferSize = {80, 25};
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    system("cls");

    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    int windowWidth = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
    int windowHeight = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;

    std::cout << windowWidth << "," << windowHeight << "\n";
    for (int y = 0; y < windowHeight; ++y)
    {
        for (int x = 0; x < windowWidth; ++x)
        {
            std::cout << "*";
        }
        std::cout << "\n";
    }

    std::string str;
    std::cin >> str;
    return 0;
}
