#include "editBox.h"

namespace screens
{
    void editBox::openStringEditBox(std::string title, std::string& data, const std::function<void(std::string newData)>& callback)
    {
        std::string newValue;
        if (editBoxSetup(title, data, newValue))
        {
            data = newValue;
        }else
        {
            newValue = data;
        }
        editBoxTearDown(newValue, callback);
    }

    void editBox::openSizeTEditBox(std::string title, size_t& data, const std::function<void(std::string)>& callback)
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
        editBoxTearDown(newValue, callback);
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
        if (newValue != "quit" && !newValue.empty())
        {
            return true;
        }
        return false;
    }

    void editBox::editBoxTearDown(std::string newData, const std::function<void(std::string)>& callback)
    {
        callback(newData);
        blockInputs = false;
    }
}
