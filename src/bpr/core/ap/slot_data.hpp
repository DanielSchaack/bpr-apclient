#pragma once
#include <nlohmann/json_fwd.hpp>
#include <unordered_set>

namespace bpr
{

    struct SlotData
    {
        std::string semver;
        int licenseGoal;
        int carGoalCount;
        std::map<int, int> amountAddedEvents;
        std::map<int, int> uniqueEventWinsAmounts;
        int uniqueEventWins;
        int uniqueCarWins;
        int finishedEventsStayFinished;
        int lockBreakables;
        int cacheBreakables;
        int includeDriveThrusJumpPoints;
        std::map<int, int> superJumpAmounts;
        std::map<int, int> smashAmounts;
        std::map<int, int> billboardAmounts;
        std::map<int, int> drivethruAmounts;
        std::map<int, int> roadRuleTimeAmounts;
        std::map<int, int> roadRuleShowtimeAmounts;
        std::map<int, int> roadRuleTimeBikeDayAmounts;
        std::map<int, int> roadRuleTimeBikeNightAmounts;
        std::map<int, int> modeBaseDifficulty = {{1, 0}, {2, 0}, {3, 0}, {4, 0}};
        bool addedLiveryItems;
        int addBurningRouteRewardCars = 0;
        int addCarbonCars = 0;
        int addLegendaryCars = 0;
        int addOnlineCars = 0;
        int addBoostSpecialCars = 0;
        int addToyCars = 0;
        int addPcpdCars = 0;
        int addBigSurfIslandCars = 0;
        int addParadiseBikes = 0;
        std::unordered_set<std::string> addedCars;
        std::unordered_set<std::string> addedEvents;
    };

    [[nodiscard]] SlotData parse_slot_data(const nlohmann::json &data);
}
