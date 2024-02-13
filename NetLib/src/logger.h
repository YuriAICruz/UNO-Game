﻿#pragma once
#include <sstream>
#include <string>
#include "../framework.h"

class NETCODE_API logger
{
public:
    static void print(std::string msg);
    static void print(const char* msg);

    static void printError(const std::string& msg);
    static void printError(const char* msg);

private:
    static std::wstring toWString(std::string value);
};
