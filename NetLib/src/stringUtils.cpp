#include "stringUtils.h"

#include <sstream>

std::vector<std::string> stringUtils::splitString(const std::string& s, char separator)
{
    std::vector<std::string> tokens;
    std::istringstream ss(s);
    std::string token;

    while (std::getline(ss, token, separator))
    {
        tokens.push_back(token);
    }

    return tokens;
}
