#include "renderer.h"

#include <vector>
#include <vector>

#include "bufferData.h"
#include "rendererException.h"
#include "elements/frame.h"

namespace elements
{
    class element;
}

namespace renderer
{
    renderer::renderer()
    {
        running = true;

        currentBuffer = &windowBuffer;
        nextBuffer = &windowBufferB;

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

    void renderer::setDirty()
    {
        dirty = true;
    }

    void renderer::forceRedraw()
    {
        forceNextFrame = true;
    }

    COORD renderer::getWindowSize() const
    {
        return lastWindowSize;
    }

    bool renderer::canEnd() const
    {
        return !running;
    }

    void renderer::exit()
    {
        running = false;
    }

    void renderer::draw()
    {
        resetScreen();

        for (int i = 0, n = elements.size(); i < n; ++i)
        {
            elements.at(i)->draw(currentBuffer);
        }

        char lastColor = 'w';
        for (int y = 0; y < lastWindowSize.Y; ++y)
        {
            for (int x = 0; x < lastWindowSize.X; ++x)
            {
                if (!forceNextFrame &&
                    currentBuffer->at(y).at(x).color == nextBuffer->at(y).at(x).color &&
                    currentBuffer->at(y).at(x).c == nextBuffer->at(y).at(x).c)
                {
                    continue;
                }

                SetConsoleCursorPosition(hConsole, COORD{
                                             static_cast<SHORT>(x),
                                             static_cast<SHORT>(y)
                                         });
                char c = currentBuffer->at(y).at(x).color;
                if (lastColor != c)
                {
                    lastColor = c;
                    setColor(c);
                }
                std::wcout << currentBuffer->at(y).at(x).c;
            }
            std::cout << "\n";
        }

        forceNextFrame = false;
        SwapBuffers();
    }

    void renderer::clear()
    {
        elements.clear();
        clearScreen();
    }

    void renderer::blank()
    {
        clearScreen();
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
        if (index >= elements.size())
        {
            std::cerr << "Element not found: " << id;
            throw std::exception("Element not found.");
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

    COORD renderer::getConsoleWindowSize()
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
        SHORT x = info.srWindow.Right - info.srWindow.Left + 1;
        SHORT y = info.srWindow.Bottom - info.srWindow.Top;
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
        system("cls");
        std::cout << std::flush;
        forceRedraw();
        clearBuffer();
    }

    void renderer::resetScreen() const
    {
        SetConsoleCursorPosition(hConsole, COORD{0, 0});

        clearBuffer();
    }

    void renderer::clearBuffer() const
    {
        for (int x = 0; x < lastWindowSize.X; ++x)
        {
            for (int y = 0; y < lastWindowSize.Y; ++y)
            {
                currentBuffer->at(y).at(x).c = L' ';
                currentBuffer->at(y).at(x).color = 'w';
            }
        }
    }

    void renderer::SwapBuffers()
    {
        std::swap(currentBuffer, nextBuffer);
    }

    void renderer::updateBuffer()
    {
        windowBuffer = std::vector<std::vector<bufferData>>(lastWindowSize.Y);
        windowBufferB = std::vector<std::vector<bufferData>>(lastWindowSize.Y);
        for (int y = 0; y < lastWindowSize.Y; ++y)
        {
            windowBuffer.at(y) = std::vector<bufferData>(lastWindowSize.X);
            windowBufferB.at(y) = std::vector<bufferData>(lastWindowSize.X);
        }
    }
}
