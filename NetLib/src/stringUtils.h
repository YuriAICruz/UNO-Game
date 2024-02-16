#pragma once
#include <vector>
#include <string>

#include "serverCommands.h"

class stringUtils
{
public:
    static std::vector<std::string> splitString(const std::string& s, char separator = NC_SEPARATOR);    
};
