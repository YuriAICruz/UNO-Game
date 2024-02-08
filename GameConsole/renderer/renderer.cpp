#include "renderer.h"

#include <vector>
#include <vector>

#include "bufferData.h"
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

        std::locale::global(std::locale("en_US.UTF-8"));

        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
        defaultAttributes = consoleInfo.wAttributes;

        lastWindowSize = getConsoleWindowSize();
        updateBuffer();

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

        char lastColor = 'w';
        for (int y = 0; y < lastWindowSize.Y; ++y)
        {
            for (int x = 0; x < lastWindowSize.X; ++x)
            {
                char c = windowBuffer.at(y).at(x).color;
                if (lastColor != c)
                {
                    lastColor = c;
                    setColor(c);
                }
                std::wcout << windowBuffer.at(y).at(x).c;
            }
            std::cout << "\n";
        }
    }

    size_t renderer::addElement(std::unique_ptr<elements::element> valuePtr)
    {
        elements.push_back(std::move(valuePtr));
        return elements.back()->getId();
    }

    elements::element* renderer::getElement(size_t id) const
    {
        int index = 0;
        for (int n = elements.size(); index < n; ++index)
        {
            if (elements.at(index)->getId() == id)
            {
                break;
            }
        }

        return elements.at(index).get();
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

    void renderer::setColor(char color)
    {
        switch (color)
        {
        case 'r':
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            return;
        case 'g':
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            return;
        case 'b':
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            return;
        case 'c':
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            return;
        case 'p':
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            return;
        case 'y':
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            return;
        }
        SetConsoleTextAttribute(hConsole, defaultAttributes);
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
                windowBuffer.at(y).at(x).c = L' ';
                windowBuffer.at(y).at(x).color = 'w';
            }
        }
    }

    void renderer::updateBuffer()
    {
        windowBuffer = std::vector<std::vector<bufferData>>(lastWindowSize.Y);
        for (int y = 0; y < lastWindowSize.Y; ++y)
        {
            windowBuffer.at(y) = std::vector<bufferData>(lastWindowSize.X);
        }
    }
}
