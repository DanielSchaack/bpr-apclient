#include "info_window.hpp"
#include <format>
#include <imgui.h>
#include "bpr/core/logger.hpp"

void InfoWindow::ToggleVisibility() {
    isVisible = !isVisible;
}

void InfoWindow::AddLogMessage(const std::string& message) {
    if (logMessages.size() >= maxLogMessages)
        logMessages.pop_front(); // remove oldest
    logMessages.push_back(std::format("{}: {}", Logger::GetDateTimeString(), message)); // add newest at the end
}

void InfoWindow::Draw(int outerWidth, int outerHeight, float uiScale) {
    if (!isVisible)
        return;

    ImGui::Begin("Log", &isVisible, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::TextUnformatted("AP Client Temp Logger. Press F3 to toggle");
    ImGui::TextUnformatted("");
    for (const auto& msg : logMessages) {
        ImGui::InputText("", (char*)msg.c_str(), msg.length(), ImGuiInputTextFlags_ReadOnly);
    }

    ImGui::End();
}