#pragma once
#include <sstream>
#include <string>
#include "../framework.h"

class NETCODE_API logger
{
public:
    struct printer
    {
    private:
        std::stringstream ss;

    public:
        printer() = default;

        template <typename T>
        printer& operator<<(const T& value)
        {
            ss << value;
            return *this;
        }

        std::string str()
        {
            return ss.str();
        }
    };

    static printer getPrinter()
    {
        return printer();
    }

    static void print(std::string msg);
    static void print(const char* msg);

    static void printError(const std::string& msg);
    static void printError(const char* msg);

private:
    static std::wstring toWString(std::string value);
};
