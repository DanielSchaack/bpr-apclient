#pragma once
#include <string>

class ModConfig {
public:
    std::string server = "archipelago.gg:38281";
    std::string slot;
    float login_window_x = 0.0f;
    float login_window_y = 0.0f;
    bool hasWindowPos = false;

    float info_window_x = 0.0f;
    float info_window_y = 0.0f;
    bool  hasInfoWindowPos = false;

    bool bannerBottomUp = false;
    float bannerHoldSeconds = 10.0f;
    float bannerFadeSeconds = 1.0f;
    float bannerPromoteIntervalSeconds = 1.0f;
    int   bannerMaxVisible = 10;

    static ModConfig& Get();

    bool Save() const;

    ModConfig(const ModConfig&) = delete;
    ModConfig& operator=(const ModConfig&) = delete;
private:
    ModConfig();
    bool Load();
};
