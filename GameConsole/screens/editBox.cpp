#include "editBox.h"

namespace screens
{
    void editBox::openStringEditBox(std::string title, std::string& data, const std::function<void()>& callback)
    {
        std::string newValue;
        if (editBoxSetup(title, data, newValue))
        {
            data = newValue;
        }
        editBoxTearDown(callback);
    }

    void editBox::openSizeTEditBox(std::string title, size_t& data, const std::function<void()>& callback)
    {
        std::string newValue;
        if (editBoxSetup(title, data, newValue))
        {
            try
            {
                data = std::stoul(newValue);
            }
            catch (const std::invalid_argument& e)
            {
                std::cerr << "ERROR: Invalid argument: " << e.what() << std::endl;
            } catch (const std::out_of_range& e)
            {
                std::cerr << "ERROR: Out of range: " << e.what() << std::endl;
            }
        }
        editBoxTearDown(callback);
    }

    bool editBox::isBlocking()
    {
        return blockInputs;
    }

    template <typename T>
    bool editBox::editBoxSetup(std::string title, T& data, std::string& newValue)
    {
        blockInputs = true;
        rdr->blank();
        std::cout << "Enter a new value for " << title << ", you can cancel by typing 'quit'" << "\n";
        std::cout << "Current value is: " << data << "\n";
        std::getline(std::cin, newValue);
        if (newValue != "quit")
        {
            return true;
        }
        return false;
    }

    void editBox::editBoxTearDown(const std::function<void()>& callback)
    {
        callback();
        blockInputs = false;
    }
}
