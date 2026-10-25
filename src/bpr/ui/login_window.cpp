#include "login_window.hpp"
#include <imgui.h>
#include <nlohmann/json.hpp>
#include <string>
#include "../app/app.hpp"
#include "bpr/core/logger.hpp"
#include "bpr/core/mod_config.hpp"
#include "bpr/ui/tracker.hpp"

LoginWindow::LoginWindow() : Window(std::string("Archipelago v") + VERSION) {
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

void DrawStatusIcon(GoalStatus s) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const float h  = ImGui::GetTextLineHeight();
    const ImVec2 p = ImGui::GetCursorScreenPos();
    const ImVec2 c(p.x + h * 0.5f, p.y + h * 0.5f);
    const float r  = h * 0.35f;

    switch (s) {
        case GoalStatus::Met: {
            const ImVec2 pts[3] = {{c.x - r, c.y}, {c.x - r * 0.25f, c.y + r * 0.8f}, {c.x + r, c.y - r * 0.8f}};
            dl->AddPolyline(pts, 3, IM_COL32(80, 220, 100, 255), ImDrawFlags_None, 2.5f);
            break;
        }
        case GoalStatus::Achievable:
            dl->AddCircleFilled(c, r, IM_COL32(240, 200, 60, 255));
            break;
        case GoalStatus::NotMet: {
            const ImU32 red = IM_COL32(230, 70, 70, 255);
            dl->AddLine({c.x - r, c.y - r}, {c.x + r, c.y + r}, red, 2.5f);
            dl->AddLine({c.x - r, c.y + r}, {c.x + r, c.y - r}, red, 2.5f);
            break;
        }
        case GoalStatus::NotAGoal: {
            break;   // empty
        }
    }
    ImGui::Dummy(ImVec2(h, h));
    ImGui::SameLine();
}

void CellRight(const std::string &s) {
    const float w = ImGui::CalcTextSize(s.c_str()).x;
    const float avail = ImGui::GetContentRegionAvail().x;
    if (avail > w)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail - w);
    ImGui::TextUnformatted(s.c_str());
}

void CellCenter(const std::string &s) {
    const float w = ImGui::CalcTextSize(s.c_str()).x;
    const float avail = ImGui::GetContentRegionAvail().x;
    if (avail > w)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail - w) * 0.5f);
    ImGui::TextUnformatted(s.c_str());
}

void DrawGoalTable() {
    constexpr ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg
                                    | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoSavedSettings;
    if (!ImGui::BeginTable("goals", 5, flags))
        return;

    ImGui::TableSetupColumn("##status");
    ImGui::TableSetupColumn("Goal");
    ImGui::TableSetupColumn("Done");
    ImGui::TableSetupColumn("Required");
    ImGui::TableSetupColumn("Available");
    ImGui::TableHeadersRow();

    for (const GoalSection& section : BuildGoalSections()) {
        ImGui::TableNextRow();
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImGuiCol_TableHeaderBg));
        ImGui::TableSetColumnIndex(1);
        ImGui::TextDisabled("%s", section.title.c_str());

        for (const GoalRow& row : section.rows) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); DrawStatusIcon(row.status);
            ImGui::TableSetColumnIndex(1); ImGui::TextUnformatted(row.label.c_str());
            ImGui::TableSetColumnIndex(2); CellCenter(row.done);
            ImGui::TableSetColumnIndex(3); CellCenter(row.required);
            ImGui::TableSetColumnIndex(4); CellCenter(row.available);
        }
    }
    ImGui::EndTable();
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
        if(App::Instance->State().HasApSaveData() && ImGui::CollapsingHeader("Goal Progress", ImGuiTreeNodeFlags_DefaultOpen))
        {
            DrawGoalTable();
            ImGui::TextWrapped("Green: condition completed");
            ImGui::TextWrapped("Yellow: condition reachable");
            ImGui::TextWrapped("Red: condition not met");
            ImGui::TextWrapped("No icon: not a condition goal");
        }
    }


    ImGui::TextWrapped("%s", message.c_str());
    ImGui::TextWrapped("%s","Press F2 to toggle visibility.");


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
