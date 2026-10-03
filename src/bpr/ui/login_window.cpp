#include "login_window.hpp"
#include <imgui.h>
#include <nlohmann/json.hpp>
#include <string>
#include "../app/app.hpp"
#include "bpr/core/logger.hpp"
#include "bpr/core/mod_config.hpp"

LoginWindow::LoginWindow() : Window(std::string("Login v") + VERSION) {
    isVisible = true;

    ModConfig& cfg = ModConfig::Get();

    strncpy_s(server, sizeof(server), cfg.server.c_str(), _TRUNCATE);
    strncpy_s(slot,   sizeof(slot),   cfg.slot.c_str(),   _TRUNCATE);
}

void LoginWindow::SaveConfig() {
    ModConfig& cfg = ModConfig::Get();
    cfg.server = server;
    cfg.slot = slot;
    if (!cfg.Save())
        Logger::Log("Failed to write config file");
}

void LoginWindow::ToggleVisibility() {
    isVisible = !isVisible;
}

void LoginWindow::Draw(int outerWidth, int outerHeight, float uiScale) {
    if (!isVisible)
        return;

    ModConfig& cfg = ModConfig::Get();

    if (!positionApplied) {
        if (cfg.hasWindowPos)
            ImGui::SetNextWindowPos(ImVec2(cfg.login_window_x, cfg.login_window_y), ImGuiCond_Always);
        positionApplied = true;
    }

    // Draw login window
    ImGui::Begin(name.c_str(), &isVisible, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::InputText("Server", server, IM_ARRAYSIZE(server));
    ImGui::InputText("Slot Name", slot, IM_ARRAYSIZE(slot));
    ImGui::InputText("Password", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password);

    if (App::Instance->State().isDisconnected()) {
        if (ImGui::Button("Connect")) {
            if (strlen(server) > 0 && strlen(slot) > 0) {
                App::Instance->State().Connect(server, slot, password);
                SetMessage("Connecting to " + std::string(server) + "...");
            }
            else {
                SetMessage("Please enter server and slot name");
            }
            SaveConfig();
        }
    }
    else {
        if (ImGui::Button("Disconnect")) {
            App::Instance->State().Disconnect();
            SetMessage("");
        }
    }


    ImGui::TextWrapped("%s", message.c_str());
    ImGui::TextWrapped("%s","Press F2 to Toggle visibility.");


    ImVec2 pos = ImGui::GetWindowPos();
    bool moved = pos.x != cfg.login_window_x || pos.y != cfg.login_window_y;
    if (moved && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        cfg.login_window_x = pos.x;
        cfg.login_window_y = pos.y;
        cfg.hasWindowPos = true;
        SaveConfig();
    }

    ImGui::End();
}


void LoginWindow::SetMessage(std::string newMessage) {
    message = newMessage;
}
