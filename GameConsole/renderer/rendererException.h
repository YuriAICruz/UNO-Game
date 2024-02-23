#pragma once
#include <exception>
#include <string>

class rendererException : std::exception
{
    std::string msg;

public:
    rendererException(const std::string msg)
    {
        this->msg = msg;
    }

    const char* what() const override
    {
        return msg.c_str();
    }
};
