#pragma once

#define API_EXPORTS

#ifdef API_EXPORTS
#define NETCODE_API __declspec(dllexport)
#else
#define NETCODE_API __declspec(dllimport)
#endif //API_EXPORTS

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
