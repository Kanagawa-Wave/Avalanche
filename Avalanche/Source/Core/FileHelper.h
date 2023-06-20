#pragma once

#include <string>

class FileHelper
{
public:
    static std::string OpenFile(const char* filter = nullptr);
    static std::string SaveFile(const char* filter = nullptr);
};
