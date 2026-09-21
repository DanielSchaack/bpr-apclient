#pragma once
#include <nlohmann/json_fwd.hpp>

namespace bpr
{
    enum class Goal {
        ReachLicense = 0,
        AquireCars = 2
    };


    struct SlotData
    {
        int goalConfig;
        int licenseGoal;
        bool lockBreakables;
        std::map<int, int> superJumpAmounts;
        std::map<int, int> smashAmounts;
        std::map<int, int> billboardAmounts;
        bool deathlink;
    };

    int AreaNameToIndex(const std::string& area);

    [[nodiscard]] SlotData parse_slot_data(const nlohmann::json &data);
}