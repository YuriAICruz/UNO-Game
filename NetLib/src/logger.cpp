#include "logger.h"
#include <sstream>
#include <utility>
#include <iostream>

bool logger::canPrintCout = false;

void logger::print(std::string msg)
{
#ifdef _DEBUG
    std::stringstream ss;
    ss << "LOG: " << msg << "\n";
    if(canPrintCout)
    {
        std::cout << ss.str();
    }
    OutputDebugString(toWString(ss.str()).c_str());
#endif // _DEBUG
}

void logger::print(const char* msg)
{
#ifdef _DEBUG
    std::stringstream ss;
    ss << "LOG: " << msg << "\n";
    if(canPrintCout)
    {
        std::cout << ss.str();
    }
    OutputDebugString(toWString(ss.str()).c_str());
#endif // _DEBUG
}

void logger::printError(const std::string& msg)
{
#ifdef _DEBUG
    std::stringstream ss;
    ss << "ERROR: " << msg << "\n";
    if(canPrintCout)
    {
        std::cout << ss.str();
    }
    OutputDebugString(toWString(ss.str()).c_str());
#endif // _DEBUG
}

void logger::printError(const char* msg)
{
#ifdef _DEBUG
    std::stringstream ss;
    ss << "ERROR: " << msg << "\n";
    if(canPrintCout)
    {
        std::cout << ss.str();
    }
    OutputDebugString(toWString(ss.str()).c_str());
#endif // _DEBUG
}

void logger::printCout(bool canPrint)
{
    canPrintCout = canPrint;
}

std::wstring logger::toWString(std::string value)
{
    return std::wstring(value.begin(), value.end()).c_str();
}
