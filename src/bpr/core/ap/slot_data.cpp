#include "slot_data.hpp"
#include "bpr/core/logger.hpp"
#include "bpr/core/map.hpp"
#include <nlohmann/json.hpp>

bpr::SlotData bpr::parse_slot_data(const nlohmann::json &data){
    bpr::SlotData slot_data;
    const bool obj = data.is_object();
    slot_data.semver = obj ? data.value("sem_ver", "") : "";
    Logger::Log(slot_data.semver);

    slot_data.goalConfig = obj ? data.value("goal_config", 0) : 0;
    Logger::Log(std::format("goalConfig: {}", slot_data.goalConfig));

    slot_data.licenseGoal = (obj ? data.value("license_goal", 1) : 1) + 2;
    Logger::Log(std::format("licenseGoal: {}", slot_data.licenseGoal));

    slot_data.deathlink = obj ? data.value("death_link", 0) : 0;
    Logger::Log(std::format("deathlink: {}", slot_data.deathlink));

    slot_data.deathlinkAmnesty = obj ? data.value("death_link_amnesty", 0) : 0;
    Logger::Log(std::format("deathlinkAmnesty: {}", slot_data.deathlinkAmnesty));

    slot_data.lockBreakables = obj ? data.value("breakable_locks", 0) : 0;
    Logger::Log(std::format("lockBreakables: {}", slot_data.lockBreakables));

    slot_data.addedLiveryItems = obj ? data.value("add_livery_items", 0) : 0;
    Logger::Log(std::format("addedLiveryItems: {}", slot_data.addedLiveryItems));

    if (obj && data.contains("smash_sanity"))
    {
        for (const auto& [areaName, amount] : data.at("smash_sanity").items())
        {
            const int areaIndex = Map::AreaNameToIndex(areaName);

            slot_data.smashAmounts[areaIndex] = amount.get<int>();
            Logger::Log(std::format("Smash-area: {} num: {}", areaIndex , amount.get<int>()));
        }
    }

    if (obj && data.contains("billboard_sanity"))
    {
        for (const auto& [areaName, amount] : data.at("billboard_sanity").items())
        {
            const int areaIndex = Map::AreaNameToIndex(areaName);

            slot_data.billboardAmounts[areaIndex] = amount.get<int>();
            Logger::Log(std::format("Billboard-area: {} num: {}", areaIndex , amount.get<int>()));
        }
    }

    if (obj && data.contains("super_jump_sanity"))
    {
        for (const auto& [areaName, amount] : data.at("super_jump_sanity").items())
        {
            const int areaIndex = Map::AreaNameToIndex(areaName);

            slot_data.superJumpAmounts[areaIndex] = amount.get<int>();
            Logger::Log(std::format("Jump-area: {} num: {}", areaIndex , amount.get<int>()));
        }
    }
    return slot_data;
}
