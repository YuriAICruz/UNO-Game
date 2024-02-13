#include "logger.h"
#include <sstream>
#include <utility>

void logger::print(std::string msg)
{
#ifdef _DEBUG
    OutputDebugString(toWString(std::move(msg)).c_str());
#endif // _DEBUG
}

void logger::print(const char* msg)
{
#ifdef _DEBUG
    OutputDebugString(toWString(std::string(msg)).c_str());
#endif // _DEBUG
}

void logger::printError(const std::string& msg)
{
#ifdef _DEBUG
    OutputDebugString(toWString(msg).c_str());
#endif // _DEBUG
}

void logger::printError(const char* msg)
{
#ifdef _DEBUG
    OutputDebugString(toWString(std::string(msg)).c_str());
#endif // _DEBUG
}

std::wstring logger::toWString(std::string value)
{
    return std::wstring(value.begin(), value.end()).c_str();
}
