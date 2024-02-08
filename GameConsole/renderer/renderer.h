#pragma once
#include <iostream>
#include <vector>
#include "elements/element.h"
#include "../wInclude.h"

namespace renderer
{
    class renderer
    {
    private:
        HANDLE hConsole;
        COORD lastWindowSize;
        bool dirty;
        std::vector<std::unique_ptr<elements::element>> elements;
        std::vector<std::vector<char>> windowBuffer;

    public:
        renderer();

        bool isDirty();
        bool canEnd() const;
        void draw();
        void addElement(std::unique_ptr<elements::element> valuePtr);
        void removeElement(size_t id);

    private:
        static BOOL consoleHandlerRoutine(DWORD dwCtrlType);
        static COORD getConsoleWindowSize();

        void clearScreen();
        void clearBuffer();
        void updateBuffer();
    };
}
