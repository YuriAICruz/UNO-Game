#pragma once
#include <functional>
#include <string>

#include "../renderer/renderer.h"

namespace screens
{
    class editBox
    {
    private:
        bool blockInputs = false;
        renderer::renderer* rdr;

    public:
        explicit editBox(renderer::renderer* rdr) : rdr(rdr) {  }
        void openStringEditBox(std::string title, std::string& data, const std::function<void(std::string newData)>& callback);
        void openSizeTEditBox(std::string title, size_t& data, const std::function<void(std::string)>& callback);
        bool isBlocking();

    private:
        template <class T>
        bool editBoxSetup(std::string title, T& data, std::string& newValue);
        void editBoxTearDown(std::string newData, const std::function<void(std::string)>& callback);
    };
}
