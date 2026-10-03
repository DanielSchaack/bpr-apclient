#include "mod_config.hpp"
#include <filesystem>
#include <fstream>
#include <format>
#include <windows.h>
#include "bpr/core/logger.hpp"


static const std::filesystem::path ConfigPath = "mods/config/bpr_ap_config.ini";

ModConfig::ModConfig() {
    Logger::Log(std::format("Config path: {}", ConfigPath.string()));
    if (!Load() && Save()) // If file is missing, write default and load it
        Load();
}

ModConfig& ModConfig::Get() {
    static ModConfig instance;
    return instance;
}

static std::string Trim(const std::string& s) {
    const char* ws = " \t\r\n";
    size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

bool ModConfig::Load() {
    std::ifstream file(ConfigPath);
    if (!file.is_open())
        return false;

    std::string line;
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';' || line[0] == '[')
            continue;

        size_t eq = line.find('=');
        if (eq == std::string::npos)
            continue;

        std::string key = Trim(line.substr(0, eq));
        std::string value = Trim(line.substr(eq + 1));

        if (key == "server")        server = value;
        else if (key == "slot")     slot = value;
        else if (key == "login_window_x") { login_window_x = std::stof(value); hasWindowPos = true; }
        else if (key == "login_window_y") { login_window_y = std::stof(value); hasWindowPos = true; }
        else if (key == "info_window_x")   { info_window_x = std::stof(value);  hasInfoWindowPos = true; }
        else if (key == "info_window_y")   { info_window_y = std::stof(value);  hasInfoWindowPos = true; }
        else if (key == "banner_bottom_up") bannerBottomUp = (value == "1" || value == "true");
        else if (key == "banner_hold_seconds")             bannerHoldSeconds = std::stof(value);
        else if (key == "banner_fade_seconds")             bannerFadeSeconds = std::stof(value);
        else if (key == "banner_promote_interval_seconds") bannerPromoteIntervalSeconds = std::stof(value);
        else if (key == "banner_max_visible")              bannerMaxVisible = std::stoi(value);
        else Logger::Log(std::format("Key not found: {}", key));
    }
    return true;
}

bool ModConfig::Save() const {
    std::error_code ec;
    std::filesystem::create_directories(ConfigPath.parent_path(), ec);
    if (ec)
        return false;

    std::ofstream file(ConfigPath, std::ios::trunc);
    if (!file.is_open())
        return false;

    file << "# Burnout Paradise Remastered - Archipelago mod config\n";

    file << "[login]\n";
    file << "server=" << server << "\n";
    file << "slot=" << slot << "\n";
    file << "login_window_x=" << login_window_x << "\n";
    file << "login_window_y=" << login_window_y << "\n";

    file << "[info]\n";
    file << "info_window_x=" << info_window_x << "\n";
    file << "info_window_y=" << info_window_y << "\n";

    file << "[banner]\n";
    file << "banner_bottom_up=" << (bannerBottomUp ? 1 : 0) << "\n";
    file << "banner_hold_seconds=" << bannerHoldSeconds << "\n";
    file << "banner_fade_seconds=" << bannerFadeSeconds << "\n";
    file << "banner_promote_interval_seconds=" << bannerPromoteIntervalSeconds << "\n";
    file << "banner_max_visible=" << bannerMaxVisible << "\n";
    return file.good();
}
