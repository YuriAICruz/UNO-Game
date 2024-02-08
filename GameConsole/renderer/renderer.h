﻿#pragma once
#include <iostream>
#include <vector>
#include "elements/element.h"
#include "bufferData.h"
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
        WORD defaultAttributes;

    public:
        renderer();

        bool isDirty();
        bool canEnd() const;
        void draw();
        size_t addElement(std::unique_ptr<elements::element> valuePtr);
        elements::element* getElement(size_t id) const;
        void removeElement(size_t id);

    private:
        std::vector<std::vector<bufferData>> windowBuffer;
        static BOOL consoleHandlerRoutine(DWORD dwCtrlType);
        static COORD getConsoleWindowSize();

        void setColor(char color);
        void clearScreen();
        void clearBuffer();
        void updateBuffer();
    };
}
