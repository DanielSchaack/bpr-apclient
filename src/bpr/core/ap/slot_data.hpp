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
        std::string semver;
        int goalConfig;
        int licenseGoal;
        int lockBreakables;
        std::map<int, int> superJumpAmounts;
        std::map<int, int> smashAmounts;
        std::map<int, int> billboardAmounts;
        bool deathlink;
        int deathlinkAmnesty;
        bool addedLiveryItems;
    };

    [[nodiscard]] SlotData parse_slot_data(const nlohmann::json &data);
}
