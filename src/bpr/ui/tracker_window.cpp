#include "tracker_window.hpp"
#include <imgui.h>
#include "bpr/core/mod_config.hpp"
#include "tracker.hpp"
#include "../app/app.hpp"

namespace {
    void DrawTrackerTable() {
        constexpr ImGuiTableFlags flags =
            ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_NoSavedSettings;
        const float footer = ImGui::GetTextLineHeightWithSpacing();
        const ImVec2 size(0.0f, ImGui::GetContentRegionAvail().y - footer);

        if (ImGui::BeginTable("tracker", 5, flags, size)) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Status");
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Done");
            ImGui::TableSetupColumn("Available");
            ImGui::TableSetupColumn("Total");
            ImGui::TableHeadersRow();

            for (const TrackerSection &section : BuildTrackerSections()) {
            ImGui::TableNextRow();
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                                    ImGui::GetColorU32(ImGuiCol_TableHeaderBg));
            ImGui::TableSetColumnIndex(1);
            ImGui::TextDisabled("%s", section.title.c_str());

            for (const TrackerRow &row : section.rows) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                DrawStatusIcon(row.status);
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(row.name.c_str());
                ImGui::TableSetColumnIndex(2);
                CellCenter(std::to_string(row.done));
                ImGui::TableSetColumnIndex(3);
                CellCenter(std::to_string(row.available));
                ImGui::TableSetColumnIndex(4);
                CellCenter(std::to_string(row.total));
            }
            }
            ImGui::EndTable();
        }
        ImGui::TextWrapped("Green: All available locations done");
        ImGui::TextWrapped("Yellow: Available locations to be done");
        ImGui::TextWrapped("Red: No locations available yet");
        ImGui::TextWrapped("No icon: No locations included");
    }
}

void TrackerWindow::ToggleVisibility() {
    isVisible = !isVisible;
}

void TrackerWindow::Draw(int outerWidth, int outerHeight, float uiScale) {
    if (!isVisible)
        return;

    ModConfig& cfg = ModConfig::Get();

    if (!positionApplied) {
        if (cfg.hasTrackerWindowPos)
            ImGui::SetNextWindowPos(ImVec2(cfg.tracker_window_x, cfg.tracker_window_y), ImGuiCond_Always);
        positionApplied = true;
    }

    ImGui::Begin(name.c_str(), &isVisible, ImGuiWindowFlags_AlwaysAutoResize);

    if (App::Instance->State().isDisconnected())
        ImGui::TextUnformatted("Connect to a slot to see progress.");
    else if (App::Instance->State().HasApSaveData())
        DrawTrackerTable();
    else
        ImGui::TextUnformatted("Save data is not ready yet.");

    ImGui::TextWrapped("%s","Press F1 to toggle visibility.");

    ImVec2 pos = ImGui::GetWindowPos();
    bool moved = pos.x != cfg.login_window_x || pos.y != cfg.login_window_y;
    if (moved && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        cfg.login_window_x = pos.x;
        cfg.login_window_y = pos.y;
        cfg.hasWindowPos = true;
        cfg.Save();
    }

    ImGui::End();
}
