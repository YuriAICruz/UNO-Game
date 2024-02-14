#pragma once
#include <exception>
#include "../../framework.h"

class ENGINE_API alreadyExistException : public std::exception
{
public:
    alreadyExistException() = default;
};
