#include "logger.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <mutex>
#include <ctime>
#include <system_error>

#include <Windows.h>
#include <ShlObj.h>



namespace Logger
{
    namespace
    {
        std::ofstream logFile;
        std::mutex logMutex;
        bool initialized = false;

        std::string GetDateTimeString()
        {
            const auto now = std::chrono::system_clock::now();
            const std::time_t nowTime =
                std::chrono::system_clock::to_time_t(now);

            std::tm localTime{};

            localtime_s(&localTime, &nowTime);

            std::ostringstream stream;

            stream << std::put_time(
                &localTime,
                "%Y-%m-%d_%H-%M-%S"
            );

            return stream.str();
        }

        std::string GetLogTimestamp()
        {
            const auto now = std::chrono::system_clock::now();
            const std::time_t nowTime =
                std::chrono::system_clock::to_time_t(now);

            std::tm localTime{};

            localtime_s(&localTime, &nowTime);

            std::ostringstream stream;

            stream << std::put_time(
                &localTime,
                "%Y-%m-%d %H:%M:%S"
            );

            return stream.str();
        }

        bool Initialize()
        {
            if (initialized)
                return logFile.is_open();

            initialized = true;

            PWSTR localAppDataPath = nullptr;

            const HRESULT result = SHGetKnownFolderPath(
                FOLDERID_LocalAppData,
                0,
                nullptr,
                &localAppDataPath
            );

            if (FAILED(result) || localAppDataPath == nullptr)
                return false;

            const std::filesystem::path logDirectory =
                std::filesystem::path(localAppDataPath)
                / L"bpr-apclient";

            CoTaskMemFree(localAppDataPath);

            std::error_code error;

            std::filesystem::create_directories(
                logDirectory,
                error
            );

            if (error)
                return false;

            const std::string filename =
                "bprap_" +
                GetDateTimeString() +
                ".log";

            const std::filesystem::path logPath =
                logDirectory / filename;

            logFile.open(
                logPath,
                std::ios::out | std::ios::app
            );

            return logFile.is_open();
        }
    }

    void Log(const std::string& message)
    {
        std::lock_guard lock(logMutex);

        if (!Initialize())
            return;

        logFile
            << "["
            << GetLogTimestamp()
            << "] "
            << message
            << '\n';
        
        // Make sure the log survives a game crash.
        logFile.flush();
        //log to console
        std::cout << "["
            << GetLogTimestamp()
            << "] "
            << message
            << std::endl;
    }
}