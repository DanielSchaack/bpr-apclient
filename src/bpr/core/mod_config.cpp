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
        else if (key == "tracker_window_x")   { tracker_window_x = std::stof(value);  hasTrackerWindowPos = true; }
        else if (key == "tracker_window_y")   { tracker_window_y = std::stof(value);  hasTrackerWindowPos = true; }
        else if (key == "login_window_x") { login_window_x = std::stof(value); hasWindowPos = true; }
        else if (key == "login_window_y") { login_window_y = std::stof(value); hasWindowPos = true; }
        else if (key == "config_window_x")   { config_window_x = std::stof(value);  hasConfigWindowPos = true; }
        else if (key == "config_window_y")   { config_window_y = std::stof(value);  hasConfigWindowPos = true; }
        else if (key == "info_window_x")   { info_window_x = std::stof(value);  hasInfoWindowPos = true; }
        else if (key == "info_window_y")   { info_window_y = std::stof(value);  hasInfoWindowPos = true; }

        else if (key == "banner_bottom_up") bannerBottomUp = (value == "1" || value == "true");
        else if (key == "banner_hold_seconds")             bannerHoldSeconds = std::stof(value);
        else if (key == "banner_fade_seconds")             bannerFadeSeconds = std::stof(value);
        else if (key == "banner_promote_interval_seconds") bannerPromoteIntervalSeconds = std::stof(value);
        else if (key == "banner_max_visible")              bannerMaxVisible = std::stoi(value);

        else if (key == "tracker_include_open_event_name")           tracker_include_open_event_name = (value == "1" || value == "true");
        else if (key == "tracker_sort_event_names_alphabetically")   tracker_sort_event_names_alphabetically = (value == "1" || value == "true");
        else if (key == "tracker_include_open_car_win_name")         tracker_include_open_car_win_name = (value == "1" || value == "true");
        else if (key == "tracker_sort_car_win_names_alphabetically") tracker_sort_car_win_names_alphabetically = (value == "1" || value == "true");
        else if (key == "tracker_include_events")                    tracker_include_events = (value == "1" || value == "true");
        else if (key == "tracker_include_car_wins")                  tracker_include_car_wins = (value == "1" || value == "true");
        else if (key == "tracker_include_breakables")                tracker_include_breakables = (value == "1" || value == "true");
        else if (key == "tracker_include_done_sections")             tracker_include_done_sections = (value == "1" || value == "true");
        else if (key == "tracker_include_available_sections")        tracker_include_available_sections = (value == "1" || value == "true");
        else if (key == "tracker_include_not_available_sections")    tracker_include_not_available_sections = (value == "1" || value == "true");
        else if (key == "tracker_include_not_included_sections")     tracker_include_not_included_sections = (value == "1" || value == "true");

        else if (key == "show_ingame_messages")            showIngameMessages = (value == "1" || value == "true");
        else if (key == "popup_progression") popupProgression = (value == "1" || value == "true");
        else if (key == "popup_useful")      popupUseful      = (value == "1" || value == "true");
        else if (key == "popup_filler")      popupFiller      = (value == "1" || value == "true");
        else if (key == "popup_trap")        popupTrap        = (value == "1" || value == "true");

        else if (key == "deathlink")                       deathlink = (value == "1" || value == "true");
        else if (key == "deathlink_amnesty")               deathlinkAmnesty = std::stoi(value);
        else if (key == "use_what_you_get") useWhatYouGet = (value == "1" || value == "true");
        else if (key == "check_goal_only_on_event_and_license") checkGoalOnlyOnEventAndLicense = (value == "1" || value == "true");
        else if (key == "allow_swaps_in_events") allowSwapInEvents = (value == "1" || value == "true");
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

    file << "[tracker]\n";
    file << "tracker_window_x=" << tracker_window_x << "\n";
    file << "tracker_window_y=" << tracker_window_y << "\n";
    file << "tracker_include_events=" << (tracker_include_events ? 1 : 0) << "\n";
    file << "tracker_include_car_wins=" << (tracker_include_car_wins ? 1 : 0) << "\n";
    file << "tracker_include_breakables=" << (tracker_include_breakables ? 1 : 0) << "\n";
    file << "tracker_include_open_event_name=" << (tracker_include_open_event_name ? 1 : 0) << "\n";
    file << "tracker_include_open_car_win_name=" << (tracker_include_open_car_win_name ? 1 : 0) << "\n";
    file << "tracker_sort_event_names_alphabetically=" << (tracker_sort_event_names_alphabetically ? 1 : 0) << "\n";
    file << "tracker_sort_car_win_names_alphabetically=" << (tracker_sort_car_win_names_alphabetically ? 1 : 0) << "\n";
    file << "tracker_include_done_sections=" << (tracker_include_done_sections ? 1 : 0) << "\n";
    file << "tracker_include_available_sections=" << (tracker_include_available_sections ? 1 : 0) << "\n";
    file << "tracker_include_not_available_sections=" << (tracker_include_not_available_sections ? 1 : 0) << "\n";
    file << "tracker_include_not_included_sections=" << (tracker_include_not_included_sections ? 1 : 0) << "\n";

    file << "[config]\n";
    file << "config_window_x=" << config_window_x << "\n";
    file << "config_window_y=" << config_window_y << "\n";

    file << "[info]\n";
    file << "info_window_x=" << info_window_x << "\n";
    file << "info_window_y=" << info_window_y << "\n";

    file << "[banner]\n";
    file << "banner_bottom_up=" << (bannerBottomUp ? 1 : 0) << "\n";
    file << "banner_hold_seconds=" << bannerHoldSeconds << "\n";
    file << "banner_fade_seconds=" << bannerFadeSeconds << "\n";
    file << "banner_promote_interval_seconds=" << bannerPromoteIntervalSeconds << "\n";
    file << "banner_max_visible=" << bannerMaxVisible << "\n";

    file << "[misc]\n";
    file << "show_ingame_messages=" << (showIngameMessages ? 1 : 0) << "\n";
    file << "popup_progression=" << (popupProgression ? 1 : 0) << "\n";
    file << "popup_useful="      << (popupUseful ? 1 : 0) << "\n";
    file << "popup_filler="      << (popupFiller ? 1 : 0) << "\n";
    file << "popup_trap="        << (popupTrap ? 1 : 0) << "\n";
    file << "deathlink=" << (deathlink ? 1 : 0) << "\n";
    file << "deathlink_amnesty=" << deathlinkAmnesty << "\n";
    file << "use_what_you_get=" << (useWhatYouGet ? 1 : 0) << "\n";
    file << "check_goal_only_on_event_and_license=" << (checkGoalOnlyOnEventAndLicense ? 1 : 0) << "\n";
    file << "allow_swaps_in_events=" << (allowSwapInEvents ? 1 : 0) << "\n";
    return file.good();
}
