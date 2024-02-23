#pragma once
#include "../wInclude.h"
#include "inputData.h"
#include "EventBus/eventBus.h"
#include "../eventIds.h"

namespace input
{
    class inputHandler
    {
    private:
        HANDLE hInput;

    public:
        INPUT_RECORD irInBuf[128];
        DWORD cNumRead;
        DWORD mode;
        std::shared_ptr<eventBus::eventBus> events;

        inputHandler(std::shared_ptr<eventBus::eventBus> events): events(events)
        {
            hInput = GetStdHandle(STD_INPUT_HANDLE);

            // Set the console mode to enable reading mouse and keyboard events
            DWORD mode;
            GetConsoleMode(hInput, &mode);
            SetConsoleMode(hInput, mode & ~ENABLE_PROCESSED_INPUT);
        }

        std::vector<inputData> readInput()
        {
            std::vector<inputData> inputs;
            if (PeekConsoleInput(hInput, irInBuf, 128, &cNumRead) && cNumRead <= 0)
            {
                return inputs;
            }

            ReadConsoleInput(hInput, irInBuf, 128, &cNumRead);

            for (DWORD i = 0; i < cNumRead; ++i)
            {
                if (irInBuf[i].EventType == KEY_EVENT && irInBuf[i].Event.KeyEvent.bKeyDown)
                {
                    inputData d;
                    d.character = irInBuf[i].Event.KeyEvent.uChar.AsciiChar;
                    if (irInBuf[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT)
                    {
                        d.left = true;
                        events->fireEvent(INPUT_LEFT, d);
                    }
                    else if (irInBuf[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT)
                    {
                        d.right = true;
                        events->fireEvent(INPUT_RIGHT, d);
                    }
                    else if (irInBuf[i].Event.KeyEvent.wVirtualKeyCode == VK_UP)
                    {
                        d.up = true;
                        events->fireEvent(INPUT_UP, d);
                    }
                    else if (irInBuf[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN)
                    {
                        d.down = true;
                        events->fireEvent(INPUT_DOWN, d);
                    }
                    else if (irInBuf[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN)
                    {
                        d.ok = true;
                        events->fireEvent(INPUT_OK, d);
                    }
                    else if (irInBuf[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE ||
                        irInBuf[i].Event.KeyEvent.wVirtualKeyCode == VK_DELETE)
                    {
                        d.cancel = true;
                        events->fireEvent(INPUT_CANCEL, d);
                    }

                    inputs.emplace_back(d);
                }
            }

            return inputs;
        }
    };
}
