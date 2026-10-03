#include "info_window.hpp"
#include <format>
#include <imgui.h>
#include "bpr/core/logger.hpp"
#include "bpr/core/mod_config.hpp"

void InfoWindow::ToggleVisibility() {
    isVisible = !isVisible;
}

void InfoWindow::AddLogMessage(const std::string& message) {
    // check if already shown
    for (const std::string& line : logMessages)
        if (line.ends_with(message))
            return;

    if (logMessages.size() >= maxLogMessages)
        logMessages.pop_front(); // remove oldest
    logMessages.push_back(std::format("{}: {}", Logger::GetDateTimeString(), message)); // add newest at the end
}

void InfoWindow::Draw(int outerWidth, int outerHeight, float uiScale) {
    if (!isVisible)
        return;

    ModConfig& cfg = ModConfig::Get();

    if (!positionApplied) {
        if (cfg.hasInfoWindowPos) {
            // Keep it on screen in case the resolution changed
            float x = std::clamp(cfg.info_window_x, 0.0f, (float)outerWidth  - 50.0f);
            float y = std::clamp(cfg.info_window_y, 0.0f, (float)outerHeight - 50.0f);
            ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
        }
        positionApplied = true;
    }

    ImGui::SetNextWindowSizeConstraints(ImVec2(700.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::Begin("Log", &isVisible, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::TextUnformatted("AP Client Temp Logger. Press F3 to toggle                            ");
    ImGui::TextUnformatted("");
    for (size_t i = 0; i < logMessages.size(); ++i) {
        const std::string& msg = logMessages[i];
        ImGui::PushID((int)i);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("##Text", (char*)msg.c_str(), msg.length() + 1, ImGuiInputTextFlags_ReadOnly);
        ImGui::PopID();
    }

    ImVec2 pos = ImGui::GetWindowPos();
    bool moved = pos.x != cfg.info_window_x || pos.y != cfg.info_window_y;
    if (moved && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        cfg.info_window_x = pos.x;
        cfg.info_window_y = pos.y;
        cfg.hasInfoWindowPos = true;
        cfg.Save();
    }

    ImGui::End();
}
