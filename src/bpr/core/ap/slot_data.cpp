#include "slot_data.hpp"
#include "bpr/core/events.hpp"
#include "bpr/core/logger.hpp"
#include "bpr/core/map.hpp"
#include "bpr/core/mod_config.hpp"
#include <nlohmann/json.hpp>

bpr::SlotData bpr::parse_slot_data(const nlohmann::json &data){
    bpr::SlotData slot_data;
    const bool obj = data.is_object();
    slot_data.semver = obj ? data.value("sem_ver", "") : "";
    Logger::Log(slot_data.semver);

    slot_data.licenseGoal = (obj ? data.value("license_goal", 0) : 0) + 1; // +1 as 0 is beginner license that is not available as a goal
    Logger::Log(std::format("licenseGoal: {}", slot_data.licenseGoal));

    slot_data.carGoalCount = obj ? data.value("car_goal_count", 0) : 0;
    Logger::Log(std::format("carGoalCount: {}", slot_data.carGoalCount));

    slot_data.cacheBreakables = obj ? data.value("cache_breakable_locations", 0) : 0;
    Logger::Log(std::format("cacheBreakables: {}", slot_data.cacheBreakables));

    slot_data.includeDriveThrusJumpPoints = obj ? data.value("add_drive_thru_jump_points", 0) : 0;
    Logger::Log(std::format("includeDriveThrusJumpPoints: {}", slot_data.includeDriveThrusJumpPoints));

    slot_data.uniqueEventWins = obj ? data.value("unique_event_wins", 0) : 0;
    Logger::Log(std::format("uniqueEventWins: {}", slot_data.uniqueEventWins));

    slot_data.uniqueCarWins = obj ? data.value("unique_car_wins", 0) : 0;
    Logger::Log(std::format("uniqueCarWins: {}", slot_data.uniqueCarWins));

    slot_data.finishedEventsStayFinished = obj ? data.value("finished_events_stay_finished", 0) : 0;
    Logger::Log(std::format("finishedEventsStayFinished: {}", slot_data.finishedEventsStayFinished));

    slot_data.lockBreakables = obj ? data.value("breakable_locks", 0) : 0;
    Logger::Log(std::format("lockBreakables: {}", slot_data.lockBreakables));

    slot_data.addedLiveryItems = obj ? data.value("add_livery_items", 0) : 0;
    Logger::Log(std::format("addedLiveryItems: {}", slot_data.addedLiveryItems));


    slot_data.addBurningRouteRewardCars = obj ? data.value("add_burning_route_reward_cars", 0) : 0;
    Logger::Log(std::format("addBurningRouteRewardCars: {}", slot_data.addBurningRouteRewardCars));

    slot_data.addCarbonCars = obj ? data.value("add_carbon_cars", 0) : 0;
    Logger::Log(std::format("addCarbonCars: {}", slot_data.addCarbonCars));

    slot_data.addLegendaryCars = obj ? data.value("add_legendary_cars", 0) : 0;
    Logger::Log(std::format("addLegendaryCars: {}", slot_data.addLegendaryCars));

    slot_data.addOnlineCars = obj ? data.value("add_online_cars", 0) : 0;
    Logger::Log(std::format("addOnlineCars: {}", slot_data.addOnlineCars));

    slot_data.addBoostSpecialCars = obj ? data.value("add_boost_special_cars", 0) : 0;
    Logger::Log(std::format("addBoostSpecialCars: {}", slot_data.addBoostSpecialCars));

    slot_data.addToyCars = obj ? data.value("add_toy_cars", 0) : 0;
    Logger::Log(std::format("addToyCars: {}", slot_data.addToyCars));

    slot_data.addPcpdCars = obj ? data.value("add_pcpd_cars", 0) : 0;
    Logger::Log(std::format("addPcpdCars: {}", slot_data.addPcpdCars));

    slot_data.addBigSurfIslandCars = obj ? data.value("add_big_surf_island_cars", 0) : 0;
    Logger::Log(std::format("addBigSurfIslandCars: {}", slot_data.addBigSurfIslandCars));

    slot_data.addParadiseBikes = obj ? data.value("add_paradise_bikes", 0) : 0;
    Logger::Log(std::format("addParadiseBikes: {}", slot_data.addParadiseBikes));


    ModConfig& cfg = ModConfig::Get();

    cfg.useWhatYouGet = obj ? data.value("use_what_you_get", 0) : 0;
    Logger::Log(std::format("useWhatYouGet: {}", cfg.useWhatYouGet));

    cfg.deathlink = obj ? data.value("death_link", 0) : 0;
    Logger::Log(std::format("deathlink: {}", cfg.deathlink));

    cfg.deathlinkAmnesty = obj ? data.value("death_link_amnesty", 0) : 0;
    Logger::Log(std::format("deathlinkAmnesty: {}", cfg.deathlinkAmnesty));


    if (obj && data.contains("amount_events"))
    {
        for (const auto& [event_name, amount] : data.at("amount_events").items())
        {
            const int event_index = Event::AddedEventNameToIndex(event_name);

            slot_data.amountAddedEvents[event_index] = amount.get<int>();
            Logger::Log(std::format("Added Events: {} num: {}", event_index , amount.get<int>()));
        }
    }

    if (obj && data.contains("added_cars"))
    {
        for (const auto& car_name : data.at("added_cars"))
        {
            slot_data.addedCars.insert(car_name.get<std::string>());
        }
        Logger::Log(std::format("Added Cars: {}", slot_data.addedCars.size()));
    }

    if (obj && data.contains("event_items"))
    {
        for (const auto& event_name : data.at("event_items"))
        {
            slot_data.addedEvents.insert(event_name.get<std::string>());
        }
        Logger::Log(std::format("Added Events: {}", slot_data.addedEvents.size()));
    }

    if (obj && data.contains("unique_event_win_goals"))
    {
        for (const auto& [event_name, amount] : data.at("unique_event_win_goals").items())
        {
            const int event_index = Event::UniqueWinsToIndex(event_name);

            slot_data.uniqueEventWinsAmounts[event_index] = amount.get<int>();
            Logger::Log(std::format("Unique Event Win Index: {} num: {}", event_index , amount.get<int>()));
        }
    }

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

    if (obj && data.contains("drive_thru_counts"))
    {
        for (const auto& [areaName, amount] : data.at("drive_thru_counts").items())
        {
            const int areaIndex = Map::AreaNameToIndex(areaName);

            slot_data.drivethruAmounts[areaIndex] = amount.get<int>();
            Logger::Log(std::format("Drivethru-area: {} num: {}", areaIndex , amount.get<int>()));
        }
    }

    if (obj && data.contains("road_rule_time_count"))
    {
        for (const auto& [areaName, amount] : data.at("road_rule_time_count").items())
        {
            const int areaIndex = Map::AreaNameToIndex(areaName);

            slot_data.roadRuleTimeAmounts[areaIndex] = amount.get<int>();
            Logger::Log(std::format("RoadRuleTime-area: {} num: {}", areaIndex , amount.get<int>()));
        }
    }

    if (obj && data.contains("road_rule_showtime_count"))
    {
        for (const auto& [areaName, amount] : data.at("road_rule_showtime_count").items())
        {
            const int areaIndex = Map::AreaNameToIndex(areaName);

            slot_data.roadRuleShowtimeAmounts[areaIndex] = amount.get<int>();
            Logger::Log(std::format("RoadRuleShowtime-area: {} num: {}", areaIndex , amount.get<int>()));
        }
    }

    if (obj && data.contains("road_rule_bike_day_count"))
    {
        for (const auto& [areaName, amount] : data.at("road_rule_bike_day_count").items())
        {
            const int areaIndex = Map::AreaNameToIndex(areaName);

            slot_data.roadRuleTimeBikeDayAmounts[areaIndex] = amount.get<int>();
            Logger::Log(std::format("BikesDay-area: {} num: {}", areaIndex , amount.get<int>()));
        }
    }

    if (obj && data.contains("road_rule_bike_night_count"))
    {
        for (const auto& [areaName, amount] : data.at("road_rule_bike_night_count").items())
        {
            const int areaIndex = Map::AreaNameToIndex(areaName);

            slot_data.roadRuleTimeBikeNightAmounts[areaIndex] = amount.get<int>();
            Logger::Log(std::format("BikesNight-area: {} num: {}", areaIndex , amount.get<int>()));
        }
    }

    if (obj && data.contains("mode_base_difficulty"))
    {
        for (const auto& [mode_name, difficulty] : data.at("mode_base_difficulty").items())
        {
            const int mode_index = Event::ModeNameToIndex(mode_name);
            if (mode_index < 0)
            {
                Logger::Log(std::format("Unknown mode in mode_base_difficulty: {}", mode_name));
                continue;
            }

            slot_data.modeBaseDifficulty[mode_index] = difficulty.get<int>();
            Logger::Log(std::format("Mode Base Difficulty Index: {} difficulty: {}", mode_index, difficulty.get<int>()));
        }
    }
    return slot_data;
}
