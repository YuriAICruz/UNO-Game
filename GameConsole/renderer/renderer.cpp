#include "renderer.h"

#include <vector>
#include <vector>

#include "rendererException.h"

namespace elements
{
    class element;
}

namespace renderer
{
    renderer::renderer()
    {
        if (!SetConsoleCtrlHandler(consoleHandlerRoutine, TRUE))
        {
            throw rendererException("Error setting up console handler.");
        }

        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        lastWindowSize = getConsoleWindowSize();
        updateBuffer();
        
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    
        SetConsoleScreenBufferSize(hConsole, lastWindowSize);
        dirty = true;
    }

    bool renderer::isDirty()
    {
        if (dirty)
        {
            dirty = false;
            return true;
        }

        auto size = getConsoleWindowSize();
        if (size.X != lastWindowSize.X || size.Y != lastWindowSize.Y)
        {
            lastWindowSize = size;
            updateBuffer();
            return true;
        }
        return false;
    }

    bool renderer::canEnd() const
    {
        return false;
    }

    void renderer::draw()
    {
        clearScreen();

        for (int i = 0, n = elements.size(); i < n; ++i)
        {
            elements.at(i)->draw(&windowBuffer);
        }

        for (int y = 0; y < lastWindowSize.Y; ++y)
        {
            for (int x = 0; x < lastWindowSize.X; ++x)
            {
                std::cout << windowBuffer.at(y).at(x);
            }
            std::cout << "\n";
        }
    }

    void renderer::addElement(std::unique_ptr<elements::element> valuePtr)
    {
        elements.push_back(std::move(valuePtr));
    }

    void renderer::removeElement(size_t id)
    {
        int index = 0;
        for (int n = elements.size(); index < n; ++index)
        {
            if (elements.at(index)->getId() == id)
            {
                break;
            }

            index++;
        }

        if (index >= elements.size())
        {
            throw std::exception("no element found");
        }
        elements.erase(elements.begin() + index);
    }

    BOOL renderer::consoleHandlerRoutine(DWORD dwCtrlType)
    {
        //Handle window events here
        return FALSE;
    }

    COORD renderer::getConsoleWindowSize()
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
        SHORT x = info.srWindow.Right - info.srWindow.Left + 1;
        SHORT y = info.srWindow.Bottom - info.srWindow.Top + 1;
        COORD size = COORD{x, y};
        return size;
    }

    void renderer::clearScreen()
    {
        COORD topLeft = {0, 0};
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO screen;
        DWORD written;

        GetConsoleScreenBufferInfo(consoleHandle, &screen);
        FillConsoleOutputCharacter(consoleHandle, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
        FillConsoleOutputAttribute(consoleHandle, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
                                   screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
        SetConsoleCursorPosition(consoleHandle, topLeft);

        clearBuffer();
    }

    void renderer::clearBuffer()
    {
        for (int x = 0; x < lastWindowSize.X; ++x)
        {
            for (int y = 0; y < lastWindowSize.Y; ++y)
            {
                windowBuffer.at(y).at(x) = ' ';
            }
        }
    }

    void renderer::updateBuffer()
    {
        windowBuffer = std::vector<std::vector<char>>(lastWindowSize.Y);
        for (int y = 0; y < lastWindowSize.Y; ++y)
        {
            windowBuffer.at(y) = std::vector<char>(lastWindowSize.X);
        }
    }
}
