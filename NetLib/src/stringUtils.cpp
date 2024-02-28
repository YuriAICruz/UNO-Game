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

char* stringUtils::findWordStart(char* rawStr, int strSize, const char* word)
{
    for (int i = 0; i < strSize; ++i)
    {
        if (rawStr[i] == word[0])
        {
            int j;
            for (j = 0; j < strlen(word); ++j)
            {
                if (rawStr[i + j] != word[j])
                {
                    break;
                }
            }

            if (j == strlen(word))
            {
                return &rawStr[i];
            }
        }
    }

    return nullptr;
}
