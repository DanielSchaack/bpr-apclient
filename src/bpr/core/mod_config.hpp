#pragma once
#include <string>

class ModConfig {
public:
    std::string server = "archipelago.gg:38281";
    std::string slot;

    float tracker_window_x = 0.0f;
    float tracker_window_y = 0.0f;
    bool  hasTrackerWindowPos = false;

    float login_window_x = 0.0f;
    float login_window_y = 0.0f;
    bool hasWindowPos = false;

    float config_window_x = 0.0f;
    float config_window_y = 0.0f;
    bool  hasConfigWindowPos = false;

    float quicktravel_window_x = 0.0f;
    float quicktravel_window_y = 0.0f;
    bool  hasQuicktravelWindowPos = false;

    float info_window_x = 0.0f;
    float info_window_y = 0.0f;
    bool  hasInfoWindowPos = false;

    bool bannerBottomUp = true;
    float bannerHoldSeconds = 10.0f;
    float bannerFadeSeconds = 1.0f;
    float bannerPromoteIntervalSeconds = 1.0f;
    int   bannerMaxVisible = 10;

    bool showIngameMessages = true;
    bool popupProgression = true;
    bool popupUseful      = true;
    bool popupFiller      = true;
    bool popupTrap        = true;

    bool deathlink = false;
    int deathlinkAmnesty = 10;
    bool useWhatYouGet = false;
    bool checkGoalOnlyOnEventAndLicense = true;
    bool allowSwapInEvents = false;

    static ModConfig& Get();

    bool Save() const;

    ModConfig(const ModConfig&) = delete;
    ModConfig& operator=(const ModConfig&) = delete;
private:
    ModConfig();
    bool Load();
};
