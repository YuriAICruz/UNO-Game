#pragma once
#include <exception>

class alreadyExistException : public std::exception
{
public:
    alreadyExistException() = default;
};
