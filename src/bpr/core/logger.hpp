#pragma once

#include <string>

namespace Logger
{
    void Log(const std::string& message);
    std::string GetDateTimeString();
}