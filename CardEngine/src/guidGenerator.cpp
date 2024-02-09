#include "pch.h"
#include "guidGenerator.h"
#include <chrono>

size_t guidGenerator::counter = 0;

size_t guidGenerator::generateGUID()
{
    // Use current time since epoch as the basis for GUID
    auto now = std::chrono::system_clock::now();
    auto sinceEpoch = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(sinceEpoch);

    // Combine current time with a counter to ensure uniqueness
    size_t guid = static_cast<size_t>(seconds.count()) << 32 | counter++;

    return guid;
}
