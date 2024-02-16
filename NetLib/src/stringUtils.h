#pragma once
#include <vector>
#include <string>

#include "serverCommands.h"
#include "../framework.h"

class NETCODE_API stringUtils
{
public:
    static std::vector<std::string> splitString(const std::string& s, char separator = NC_SEPARATOR);    
};
