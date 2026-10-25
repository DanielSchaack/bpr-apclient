#include "quicktravel_window.hpp"
#include <imgui.h>
#include <format>
#include <string>
#include <vector>
#include "../app/app.hpp"
#include "bpr/core/mod_config.hpp"
#include "bpr/hooks/structure/detours.hpp"

namespace {
    struct Entry {
        uint64_t id;
        std::string label;
        const float* position;
    };
 
    const char* DriveThruTypeToName(Map::DriveThruType type) {
        switch (type) {
            case Map::DriveThruType::JUNKYARD: return "Junkyard";
            // TODO: add the remaining DriveThruType enumerators here
            default: return "Drive-Thru";
        }
    }
}


void QuickTravelWindow::ToggleVisibility() {
    isVisible = !isVisible;
}

void QuickTravelWindow::Draw(int outerWidth, int outerHeight, float uiScale) {
    if (!isVisible)
        return;

    ModConfig& cfg = ModConfig::Get();
 
    if (!positionApplied) {
        if (cfg.hasQuicktravelWindowPos) {
            float x = std::clamp(cfg.quicktravel_window_x, 0.0f, (float)outerWidth  - 50.0f);
            float y = std::clamp(cfg.quicktravel_window_y, 0.0f, (float)outerHeight - 50.0f);
            ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
        }
        positionApplied = true;
    }

    // Build the list of selectable entries
    std::vector<Entry> entries;
    const auto& obtained = App::Instance->State().GetSaveData().obtained_drivethru_jumppoints;
    entries.reserve(obtained.size());
    for (uint64_t id : obtained) {
        const Map::DriveThruInfo* info = Map::FindDriveThruByID(id);
        if (!info)
            continue;
        entries.push_back({
            id,
            std::format("{} at {} ({})",
                DriveThruTypeToName(info->type),
                info->junction,
                Map::AreaTypeToName(info->area)),
            info->Position
        });
    }
 
    // Keep the selection valid: drop it if its entry vanished, default to the first entry.
    const Entry* selected = nullptr;
    for (const Entry& e : entries) {
        if (e.id == selectedId) { selected = &e; break; }
    }
    if (!selected && !entries.empty()) {
        selected = &entries.front();
        selectedId = selected->id;
    }
    if (entries.empty())
        selectedId = 0;
 
    ImGui::SetNextWindowSizeConstraints(ImVec2(500.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::Begin(name.c_str(), &isVisible, ImGuiWindowFlags_AlwaysAutoResize);
 
    ImGui::BeginDisabled(entries.empty());
 
    const char* preview = selected ? selected->label.c_str() : "No jump points available";
    if (ImGui::BeginCombo("Destination", preview)) {
        for (const Entry& e : entries) {
            bool isSelected = (e.id == selectedId);
            if (ImGui::Selectable(e.label.c_str(), isSelected))
                selectedId = e.id;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
 
    if (ImGui::Button("Teleport")) {
        if (selected) {
            float pos[4];
            std::copy(selected->position, selected->position + 4, pos);
            Filler::QueueTeleport(pos);
        }
    }
 
    ImGui::EndDisabled();
 
    ImGui::TextWrapped("%s", "Press F4 to toggle visibility.");
    ImGui::End();

    cfg.Save();
}
