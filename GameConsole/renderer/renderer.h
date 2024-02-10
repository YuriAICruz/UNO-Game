#pragma once
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
        bool running;
        bool forceNextFrame = true;

    public:
        renderer();

        bool isDirty();
        void setDirty();
        void forceRedraw();
        COORD getWindowSize() const;
        bool canEnd() const;
        void exit();
        void draw();
        void clear();
        void blank();


        template <typename T, typename... Args>
        size_t addElement(Args... args)
        {
            elements.emplace_back(std::make_unique<T>(args...));
            return elements.back()->getId();
        }

        elements::element* getElement(size_t id) const;
        void removeElement(size_t id);

    private:
        std::vector<std::vector<bufferData>> windowBuffer;
        std::vector<std::vector<bufferData>> windowBufferB;
        std::vector<std::vector<bufferData>>* currentBuffer;
        std::vector<std::vector<bufferData>>* nextBuffer;
        static BOOL consoleHandlerRoutine(DWORD dwCtrlType);
        static COORD getConsoleWindowSize();

        void setColor(char color);
        void clearScreen();
        void resetScreen();
        void clearBuffer();
        void SwapBuffers();
        void updateBuffer();
    };
}
