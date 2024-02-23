#pragma once
#include <fstream>
#include <string>
#include "element.h"

namespace elements
{
    class fileRead : public element
    {
    public:
        fileRead(const COORD& pos, const char& c, const std::string& filePath):
            element(pos, COORD{0, 0}, ' ', c)
        {
            std::wifstream inputFile(filePath);

            if (!inputFile.is_open())
            {
                throw std::exception("File is Open");
            }

            std::wstring line;
            while (std::getline(inputFile, line))
            {
                std::vector<wchar_t> lineData;

                for (wchar_t wc : line)
                {
                    lineData.push_back(wc);
                }

                fileData.push_back(lineData);
            }

            inputFile.close();
        }

        void draw(std::vector<std::vector<renderer::bufferData>>* buffer) const override;

    private:
        std::vector<std::vector<wchar_t>> fileData;
    };
}
