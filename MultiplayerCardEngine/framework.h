#pragma once

#ifdef MCARD_ENGINE_EXPORTS
#define NET_ENGINE_API __declspec(dllexport)
#else
#define NET_ENGINE_API __declspec(dllimport)
#endif //MCARD_ENGINE_EXPORTS

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
