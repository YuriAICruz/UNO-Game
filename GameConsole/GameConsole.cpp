#include <iostream>
#include <Windows.h>

HANDLE hConsole;

void redraw(COORD size)
{
    system("cls");

    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    int windowWidth = size.X;
    int windowHeight = size.Y;

    std::cout << windowWidth << "," << windowHeight << "\n";
    for (int y = 0; y < windowHeight; ++y)
    {
        for (int x = 0; x < windowWidth; ++x)
        {
            std::cout << "*";
        }
        std::cout << "\n";
    }
}

COORD GetConsoleWindowSize()
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    SHORT x = info.srWindow.Right - info.srWindow.Left + 1;
    SHORT y = info.srWindow.Bottom - info.srWindow.Top + 1;
    COORD size = COORD{x, y};
    return size;
}

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
    std::cout << dwCtrlType << std::endl;
    return FALSE;
}

int main()
{
    if (!SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE))
    {
        std::cerr << "Error setting up console handler." << std::endl;
        return 1;
    }

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD bufferSize = {80, 25};
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    COORD initialSize = GetConsoleWindowSize();
    redraw(initialSize);

    while (true)
    {
        COORD currentSize = GetConsoleWindowSize();

        // Compare current size with initial size
        if (currentSize.X != initialSize.X || currentSize.Y != initialSize.Y)
        {
            // Window resized
            std::cout << "Window resized." << std::endl;
            // Update initial size
            initialSize = currentSize;
            redraw(initialSize);
        }
    }
    return 0;
}
