#include "config_window.hpp"
#include <algorithm>
#include <imgui.h>
#include "bpr/core/logger.hpp"
#include "bpr/core/mod_config.hpp"
#include "bpr/app/app.hpp"

bool EditFloat(const char* label, float& value, float minValue, float maxValue, float speed = 0.1f) {
    ImGui::DragFloat(label, &value, speed, minValue, maxValue, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    if (value < minValue) value = minValue; // guard against typed values
    return ImGui::IsItemDeactivatedAfterEdit();
}

bool EditInt(const char* label, int& value, int minValue, int maxValue) {
    ImGui::DragInt(label, &value, 0.2f, minValue, maxValue, "%d", ImGuiSliderFlags_AlwaysClamp);
    if (value < minValue) value = minValue;
    return ImGui::IsItemDeactivatedAfterEdit();
}

// Describes the current selection for the combo's preview text.
std::string PopupPreview(const ModConfig& c) {
    if (c.popupProgression && c.popupUseful && c.popupFiller && c.popupTrap)
        return "All items";
    if (c.popupProgression && c.popupUseful && !c.popupFiller && !c.popupTrap)
        return "Progression + Useful";
    if (c.popupProgression && !c.popupUseful && !c.popupFiller && !c.popupTrap)
        return "Progression only";
    if (!c.popupProgression && !c.popupUseful && !c.popupFiller && !c.popupTrap)
        return "None";

    std::string s;
    auto add = [&](bool on, const char* n) {
        if (!on) return;
        if (!s.empty()) s += " + ";
        s += n;
    };
    add(c.popupProgression, "Progression");
    add(c.popupUseful, "Useful");
    add(c.popupFiller, "Filler");
    add(c.popupTrap, "Traps");
    return "Custom: " + s;
}

void SetPopups(ModConfig& c, bool prog, bool useful, bool filler, bool trap) {
    c.popupProgression = prog;
    c.popupUseful = useful;
    c.popupFiller = filler;
    c.popupTrap = trap;
}

void ConfigWindow::ToggleVisibility() {
    isVisible = !isVisible;
}

void ConfigWindow::Draw(int outerWidth, int outerHeight, float uiScale) {
    if (!isVisible)
        return;

    ModConfig& cfg = ModConfig::Get();
    bool dirty = false;

    if (!positionApplied) {
        if (cfg.hasConfigWindowPos) {
            float x = std::clamp(cfg.config_window_x, 0.0f, (float)outerWidth  - 50.0f);
            float y = std::clamp(cfg.config_window_y, 0.0f, (float)outerHeight - 50.0f);
            ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
        }
        positionApplied = true;
    }

    // ImGui::SetNextWindowSizeConstraints(ImVec2(700.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::Begin(name.c_str(), &isVisible, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::SeparatorText("Banner");
    dirty |= ImGui::Checkbox("Stack Messages from bottom up", &cfg.bannerBottomUp);
    dirty |= EditFloat("Hold Messages in s", cfg.bannerHoldSeconds, 0.1f, 600.0f);
    dirty |= EditFloat("Fade Messages out in s", cfg.bannerFadeSeconds, 0.01f, 60.0f, 0.05f);
    dirty |= EditFloat("Time between Messages in s", cfg.bannerPromoteIntervalSeconds, 0.01f, 60.0f, 0.05f);
    dirty |= EditInt("Max visible Messages", cfg.bannerMaxVisible, 1, 50);


    ImGui::SeparatorText("Tracker");
    ImGui::TextDisabled("Sections");
    dirty |= ImGui::Checkbox("Show Events",                  &cfg.tracker_include_events);
    dirty |= ImGui::Checkbox("Show Car Wins",                &cfg.tracker_include_car_wins);
    dirty |= ImGui::Checkbox("Show Breakables",              &cfg.tracker_include_breakables);
    dirty |= ImGui::Checkbox("Show Open Event Wins",         &cfg.tracker_include_open_event_name);
    dirty |= ImGui::Checkbox("Show Open Car Wins",           &cfg.tracker_include_open_car_win_name);

    ImGui::TextDisabled("Rows by state");
    dirty |= ImGui::Checkbox("Show Done",                    &cfg.tracker_include_done_sections);
    dirty |= ImGui::Checkbox("Show Available",               &cfg.tracker_include_available_sections);
    dirty |= ImGui::Checkbox("Show Not Available",           &cfg.tracker_include_not_available_sections);
    dirty |= ImGui::Checkbox("Show Not Included",            &cfg.tracker_include_not_included_sections);

    ImGui::TextDisabled("Sorting");
    dirty |= ImGui::Checkbox("Sort Open Event Wins A-Z",     &cfg.tracker_sort_event_names_alphabetically);
    dirty |= ImGui::Checkbox("Sort Open Car Wins A-Z",       &cfg.tracker_sort_car_win_names_alphabetically);

    ImGui::SeparatorText("Misc");

    dirty |= ImGui::Checkbox("Show Items Received Messages", &cfg.showIngameMessages);
    if (ImGui::BeginCombo("Item Popups", PopupPreview(cfg).c_str())) {
        if (ImGui::Selectable("All items"))            { SetPopups(cfg, true, true, true, true);     dirty = true; }
        if (ImGui::Selectable("Progression + useful")) { SetPopups(cfg, true, true, false, true);   dirty = true; }
        if (ImGui::Selectable("Progression only"))     { SetPopups(cfg, true, false, false, true);  dirty = true; }
        ImGui::Separator();
        dirty |= ImGui::Checkbox("Progression", &cfg.popupProgression);
        dirty |= ImGui::Checkbox("Useful",      &cfg.popupUseful);
        dirty |= ImGui::Checkbox("Filler",      &cfg.popupFiller);
        dirty |= ImGui::Checkbox("Traps",       &cfg.popupTrap);
        ImGui::EndCombo();
    }

    if (ImGui::Checkbox("Deathlink", &cfg.deathlink)) {
        dirty = true;
        App::Instance->Network().ToggleDeathlink(cfg.deathlink);
    }

    dirty |= EditInt("Deathlink amnesty", cfg.deathlinkAmnesty, 1, 30);
    dirty |= ImGui::Checkbox("Use the Car You Get", &cfg.useWhatYouGet);
    dirty |= ImGui::Checkbox("Check goal only on event win and license upgrade", &cfg.checkGoalOnlyOnEventAndLicense);
    dirty |= ImGui::Checkbox("Allow car swaps in events", &cfg.allowSwapInEvents);

    ImGui::Separator();
    ImGui::TextWrapped("%s", "Press F3 to toggle visibility.");

    ImVec2 pos = ImGui::GetWindowPos();
    bool moved = pos.x != cfg.config_window_x || pos.y != cfg.config_window_y;
    if (moved && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        cfg.config_window_x = pos.x;
        cfg.config_window_y = pos.y;
        cfg.hasConfigWindowPos = true;
        dirty = true;
    }

    ImGui::End();

    if (dirty && !cfg.Save())
        Logger::Log("Failed to write config file");
}
