#include "slot_data.hpp"
#include <nlohmann/json.hpp>

int bpr::AreaNameToIndex(const std::string& area)
{
    const std::map<std::string, int> areaIndices = {
        {"Palm Bay Heights", 0},
        {"Silver Lake", 1},
        {"Harbor Town", 2},
        {"White Mountain", 3},
        {"Downtown Paradise", 4},
        {"Big Surf Island", 5},
    };

    return areaIndices.at(area);
}

bpr::SlotData bpr::parse_slot_data(const nlohmann::json &data){
    bpr::SlotData slot_data;
    const bool obj = data.is_object();
    slot_data.goalConfig = obj ? data.value("goal_config", 0) : 0;
    slot_data.licenseGoal = (obj ? data.value("license_goal", 1) : 1) + 2;
    slot_data.deathlink = obj ? data.value("death_link", false) : false;
    slot_data.lockBreakables = obj ? data.value("breakable_locks", false) : false;

    if (obj && data.contains("smash_sanity"))
    {
        for (const auto& [areaName, amount] : data.at("smash_sanity").items())
        {
            const int areaIndex = AreaNameToIndex(areaName);

            slot_data.smashAmounts[areaIndex] = amount.get<int>();
        }
    }

    if (obj && data.contains("billboard_sanity"))
    {
        for (const auto& [areaName, amount] : data.at("billboard_sanity").items())
        {
            const int areaIndex = AreaNameToIndex(areaName);

            slot_data.billboardAmounts[areaIndex] = amount.get<int>();
        }
    }

    if (obj && data.contains("super_jump_sanity"))
    {
        for (const auto& [areaName, amount] : data.at("super_jump_sanity").items())
        {
            const int areaIndex = AreaNameToIndex(areaName);

            slot_data.superJumpAmounts[areaIndex] = amount.get<int>();
        }
    }
    return slot_data;
}
