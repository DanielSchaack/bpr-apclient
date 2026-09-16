#pragma once
#include <nlohmann/json_fwd.hpp>


enum class Goal {
    ReachLicense = 0,
    AquireCars = 2
};


struct SlotData
{
    int licenseLevel;
    int CarGoalAmount;

    int superJumpAmount;
    int smashAmount;
    int billboardAmount;

    bool deathlink;
};

[[nodiscard]] SlotData parse_slot_data(const nlohmann::json &data);
