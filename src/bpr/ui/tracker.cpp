#include "tracker.hpp"
#include <array>
#include <cstdint>
#include <format>
#include <string>
#include <sys/stat.h>
#include "../app/app.hpp"
#include "bpr/core/events.hpp"
#include "bpr/core/mod_config.hpp"
#include "bpr/core/vehicles.hpp"

namespace {
    constexpr std::array<const char*, 7> kLicenseNames = {"Beginner", "D Class", "C Class", "B Class", "A Class", "Burnout", "Burnout Elite"};
    constexpr std::array<int, 7> kUniqueNeeded = {0, 2, 7, 15, 26, 40, 120};
    constexpr std::array<int, 7> kWinsNeeded   = {0, 2, 9, 24, 50, 90, 210};

    constexpr int kLicenseEventTypes = 5;

    std::string LicenseName(int n) {
        if (n >= 0 && n <= 6) return kLicenseNames[n];
        return n > 6 ? "Elite complete" : "?";
    }

    bool HasEventWinsLogic(int wins, int eventCount) {
        return wins <= eventCount;
    }

    int MaxReachableLicense(int availableEvents) {
        int best = 0;
        for (int n = 0; n < 7; ++n) {
            if (!HasEventWinsLogic(kUniqueNeeded[n], availableEvents))
                break;
            best = n;
        }
        return best;
    }

    // Met = done, Achievable = enough events unlocked to get there, NotMet = not yet
    GoalStatus Classify(int done, int required, int available, const ModConfig& config) {
        if(required <= 0)
            return config.tracker_include_not_included_sections ? GoalStatus::NotAGoal : GoalStatus::NotIncluded;
        if (done >= required)
            return GoalStatus::Met;
        if (available >= required)
            return GoalStatus::Achievable;
        return GoalStatus::NotMet;
    }

    template <std::ranges::input_range R, class Pred>
        requires std::convertible_to<std::ranges::range_value_t<R>, uint64_t>
    int CountCars(const R& ids, Pred pred) {
        int n = 0;
        for (uint64_t id : ids)
            if (const Vehicle::VehicleInfo* info = Vehicle::FindVehicleByID(id); info && pred(*info))
                ++n;
        return n;
    }

    // Burning Routes unlock with their car: only Unlock, Sponsor and ShutdownRival cars have one.
    int CountBurningRoutes(const bpr::SaveData& save) {
      return CountCars(
          save.obtained_paradise_cars, [](const Vehicle::VehicleInfo &v) {
            return v.category == Vehicle::VehicleCategory::ParadiseCars &&
                   (v.unlock_type == Vehicle::UnlockType::Unlock ||
                    v.unlock_type == Vehicle::UnlockType::Sponsor ||
                    v.unlock_type == Vehicle::UnlockType::ShutdownRival);
          });
    }
}

std::vector<GoalSection> BuildGoalSections() {
    auto &state = App::Instance->State();
    const auto &save = state.GetSaveData();
    const auto &slot = state.GetSlotData();

    auto& config = ModConfig::Get();

    // Unlocked events per type
    std::array<int, 8> available{};
    for (uint64_t id : save.obtained_car_events)
        if (const Event::EventInfo *e = Event::FindByEventId(static_cast<int32_t>(id)))
            available[static_cast<uint8_t>(e->type)]++;
    for (uint64_t id : save.obtained_bike_events)
        if (const Event::EventInfo *e = Event::FindByBikeEventId(static_cast<int32_t>(id)))
            available[static_cast<uint8_t>(e->type)]++;

    available[static_cast<uint8_t>(Event::EventType::BURNING_ROUTE)] = CountBurningRoutes(save);

    std::array<int, 8> done{};
    int eventsDone = 0, eventsAvailable = 0;
    for (int i = 0; i < 8; ++i) {
        done[i] = save.GetEventWins(static_cast<Event::EventType>(i));
            if (i < kLicenseEventTypes) {
                eventsDone += done[i];
                eventsAvailable += available[i];
            }
        }

    const int requiredWins = kUniqueNeeded[slot.licenseGoal];
    const int reachableLicence = MaxReachableLicense(eventsAvailable);

    const GoalStatus winsStatus =
        reachableLicence >= slot.licenseGoal ? GoalStatus::Met
        : HasEventWinsLogic(requiredWins, eventsAvailable)
            ? GoalStatus::Achievable
            : GoalStatus::NotMet;

    const std::string na = "-";
    std::vector<GoalSection> sections;

    // License
    sections.push_back(
        {"License",
         {
             {"License", LicenseName(save.license), LicenseName(slot.licenseGoal),
              LicenseName(reachableLicence),
              save.license >= slot.licenseGoal ? GoalStatus::Met
              : winsStatus == GoalStatus::Achievable ? GoalStatus::Achievable
                                                     : GoalStatus::NotMet},
         }});

    // Events
    GoalSection events{slot.finishedEventsStayFinished ? "Unique Event Wins" : "Event Wins", {}};
    events.rows.push_back({"Total Event Wins", std::to_string(eventsDone),
                           std::to_string(requiredWins),
                           std::to_string(eventsAvailable), winsStatus});

    for (const auto &[i, required] : slot.uniqueEventWinsAmounts) {
        if (i < 0 || i >= 8 || required <= 0)
            continue;
        auto status = Classify(done[i], required, available[i], config);
        if(status != GoalStatus::NotIncluded)
            events.rows.push_back(
                {std::format("{} Event Wins",
                            Event::to_string(static_cast<Event::EventType>(i))),
                std::to_string(done[i]), std::to_string(required),
                std::to_string(available[i]), status});
    }
    sections.push_back(std::move(events));

    // Cars
    GoalSection carSection{"Collected Cars", {}};
    auto addRow = [&](const char *label, int received, int available, const ModConfig& config) {
        GoalStatus status;
        if(available <= 0)
            status = config.tracker_include_not_included_sections ? GoalStatus::NotAGoal : GoalStatus::NotIncluded;
        else
            status = received >= available ? GoalStatus::Met : GoalStatus::NotAGoal;

        if (status != GoalStatus::NotIncluded)
            carSection.rows.push_back({label, std::to_string(received), na,
                                    std::move(std::to_string(available)),
                                    status});
    };

    const int receivedCars = static_cast<int>(save.obtained_paradise_cars.size());
    const int brCars = slot.amountAddedEvents.at(Event::AddedEventNameToIndex("Burning Route Event")) + 1; // + 1 for starter car
    const int brrCars = slot.addBurningRouteRewardCars;
    const int carbonCars = slot.addCarbonCars;

    carSection.rows.push_back(
        {"Total Paradise Cars", std::to_string(receivedCars),
         std::to_string(slot.carGoalCount), std::to_string(brCars + brrCars + carbonCars),
         receivedCars >= slot.carGoalCount ? GoalStatus::Met : GoalStatus::NotMet});

    addRow("Burning Route Cars", CountBurningRoutes(save), brCars, config);

    addRow("Burning Route Reward Cars",
           CountCars(save.obtained_cars,
                     [](const Vehicle::VehicleInfo &v) {
                     return v.category == Vehicle::VehicleCategory::ParadiseCars &&
                            v.unlock_type == Vehicle::UnlockType::Gift;
                     }),
           brrCars,
           config);

    addRow("Carbon Cars",
           CountCars(save.obtained_cars,
                     [](const Vehicle::VehicleInfo &v) {
                     return v.category == Vehicle::VehicleCategory::ParadiseCars &&
                            v.unlock_type == Vehicle::UnlockType::Trophy;
                     }),
           carbonCars,
           config);

    struct CategoryRow { const char* label; int addedCount; Vehicle::VehicleCategory category; };
    const CategoryRow categories[] = {
        {"Legendary Cars", slot.addLegendaryCars, Vehicle::VehicleCategory::LegendaryCars},
        {"Online Cars", slot.addOnlineCars, Vehicle::VehicleCategory::OnlineCars},
        {"Boost Special Cars", slot.addBoostSpecialCars, Vehicle::VehicleCategory::BoostSpecialCars},
        {"Toy Vehicles", slot.addToyCars, Vehicle::VehicleCategory::ToyVehicles},
        {"PCPD Cars", slot.addPcpdCars, Vehicle::VehicleCategory::CopCars},
        {"Big Surf Island Cars", slot.addBigSurfIslandCars, Vehicle::VehicleCategory::BigSurfIslandCars},
        {"Paradise Bikes", slot.addParadiseBikes, Vehicle::VehicleCategory::ParadiseBikes}, };

    for (const CategoryRow& c : categories) {
      addRow(c.label,
             CountCars(save.obtained_cars,
                       [&](const Vehicle::VehicleInfo &v) {
                         return v.category == c.category;
                       }),
             c.addedCount,
             config);
    }
    sections.push_back(std::move(carSection));
    return sections;
}


int Lookup(const std::map<int, int>& m, int key) {
    const auto it = m.find(key);
    return it != m.end() ? it->second : 0;
}

int BreakableTotal(const bpr::SlotData &s, int area, int type) {
    switch (type) {
    case 0:
        return Lookup(s.superJumpAmounts, area);
    case 1:
        return Lookup(s.smashAmounts, area);
    case 2:
        return Lookup(s.billboardAmounts, area);
    case 3:
        return Lookup(s.drivethruAmounts, area);
    case 4:
        return Lookup(s.roadRuleTimeAmounts, area);
    case 5:
        return Lookup(s.roadRuleShowtimeAmounts, area);
    case 6:
        return Lookup(s.roadRuleTimeBikeDayAmounts, area);
    case 7:
        return Lookup(s.roadRuleTimeBikeNightAmounts, area);
    }
    return 0;
}

// green: done, nothing left | orange: something available | red: nothing available to be done | none: no locations included
GoalStatus TrackerStatus(int done, int available, int total, const ModConfig& config) {
    if (total <= 0)
        return config.tracker_include_not_included_sections ? GoalStatus::NotAGoal : GoalStatus::NotIncluded;
    if (available <= 0)
        return config.tracker_include_not_available_sections ? GoalStatus::NotMet : GoalStatus::NotIncluded;
    if (available > 0 && done < available)
        return config.tracker_include_available_sections ? GoalStatus::Achievable : GoalStatus::NotIncluded;

    return config.tracker_include_done_sections ? GoalStatus::Met : GoalStatus::NotIncluded;
}

TrackerRow MakeRow(std::string name, int done, int available, int total, GoalStatus status) {
    return {std::move(name), done, available, total, status};
}

// Unlocked events per type (Burning Route comes from the received cars)
std::array<int, 8> UnlockedEvents(const bpr::SaveData& save) {
    std::array<int, 8> unlocked{};
    for (uint64_t id : save.obtained_car_events)
        if (const Event::EventInfo* e = Event::FindByEventId(static_cast<int32_t>(id)))
            unlocked[static_cast<uint8_t>(e->type)]++;
    for (uint64_t id : save.obtained_bike_events)
        if (const Event::EventInfo* e = Event::FindByBikeEventId(static_cast<int32_t>(id)))
            unlocked[static_cast<uint8_t>(e->type)]++;
    unlocked[static_cast<uint8_t>(Event::EventType::BURNING_ROUTE)] = CountBurningRoutes(save);
    return unlocked;
}


std::vector<TrackerSection> BuildTrackerSections() {
    auto& state      = App::Instance->State();
    const auto& save = state.GetSaveData();
    const auto& slot = state.GetSlotData();

    auto& config = ModConfig::Get();

    std::vector<TrackerSection> sections;

    // Events
    if(config.tracker_include_events)
    {
        const std::array<int, 8> unlocked = UnlockedEvents(save);
        TrackerSection events{"Events", {}};
        for (int i = 0; i < 8; ++i) {
            const auto type = static_cast<Event::EventType>(i);
            const int done  = save.GetEventWins(type) ;
            const int available = unlocked[i] + (type == Event::EventType::BIG_SURF_ISLAND && save.HasCar(0xA55EBC7BF8700000) ? 1 : 0);
            const int total = Lookup(slot.amountAddedEvents, i) + (type == Event::EventType::BURNING_ROUTE ? 1 : 0); // +1 for starter car
            auto status = TrackerStatus(done, available, total, config);

            if((type == Event::EventType::BURNING_RIDE || type == Event::EventType::MIDNIGHT_RIDE) && status == GoalStatus::Achievable && !save.HasAnyBike())
                status = config.tracker_include_not_available_sections ? GoalStatus::NotMet : GoalStatus::NotIncluded;

            if(status != GoalStatus::NotIncluded)
                events.rows.push_back(MakeRow(std::string(Event::to_string(type)), done, available, total, status));
        }

        if(!events.rows.empty())
            sections.push_back(std::move(events));
    }

    // Car wins per boost type
    if(config.tracker_include_car_wins)
    {
        static constexpr std::array<const char*, 5> kBoostNames = {"Speed", "Crash", "Stunt", "Special", "Bike"};

        std::array<int, 5> total{}, available{};
        for (const Vehicle::VehicleInfo& v : Vehicle::vehicleInfos) {

            const std::string name(v.proper_name);
            const bool owned = save.HasCar(v.id);
            const bool included = owned || slot.addedCars.contains(name) || slot.addedEvents.contains(name);
            if (!included)
                continue;

            const int b = Vehicle::BoostTypeToIndex(v.boost_type);
            ++total[b];
            if (owned)
                ++available[b];
        }

        TrackerSection cars{"Car Wins per Boost Type", {}};
        for (int b = 0; b < 5; ++b)
        {
            auto totalCarWins =
                save.GetCarWins(static_cast<Vehicle::BoostType>(b));
            auto status = TrackerStatus(totalCarWins, available[b], total[b], config);

            if(static_cast<Vehicle::BoostType>(b) == Vehicle::BoostType::Bike && status == GoalStatus::Achievable && !save.HasAnyBikeEvent())
                status = config.tracker_include_not_available_sections ? GoalStatus::NotMet : GoalStatus::NotIncluded;

            if (status != GoalStatus::NotIncluded)
                cars.rows.push_back(MakeRow(kBoostNames[b], totalCarWins,
                                        available[b], total[b], status));
        }

        if(!cars.rows.empty())
            sections.push_back(std::move(cars));
    }

    // Breakables: one section per area, one row per type
    if(config.tracker_include_car_wins)
    {
        for (size_t a = 0; a < Map::areaIndex.size(); ++a) {
            TrackerSection area{std::string(Map::areaIndex[a]), {}};
            for (size_t t = 0; t < Map::typeIndex.size(); ++t) {
                const int total  = BreakableTotal(slot, static_cast<int>(a), static_cast<int>(t));
                const int sent   = save.breakable_counts[a][t];
                const int cached = static_cast<int>(save.deferred_breakables[a][t].size());
                const int done   = sent > cached ? sent : cached;
                const bool owned = slot.lockBreakables == 0 || save.breakable_owned[a][t];

                const int available = owned ? total : 0;

                auto status = TrackerStatus(done, available, total, config);

                if((t == 6 || t == 7) && status == GoalStatus::Achievable && !save.HasAnyBike())
                    status = config.tracker_include_not_available_sections ? GoalStatus::NotMet : GoalStatus::NotIncluded;

                if(status != GoalStatus::NotIncluded)
                    area.rows.push_back(MakeRow(std::string(Map::typeIndex[t]), done, available, total, status));
            }

            if(!area.rows.empty())
                sections.push_back(std::move(area));
        }
    }

    return sections;
}

std::vector<std::string> GetOpenEventWins() {
    const auto& save = App::Instance->State().GetSaveData();
    auto& config = ModConfig::Get();
    std::vector<std::string> open;

    for (uint64_t id : save.obtained_events)
    {
        int eventId = 0;
        const Event::EventInfo* e;
        if(id > 1000000)
            e = Event::FindByBikeEventId(static_cast<int32_t>(id));
        else
            e = Event::FindByEventId(static_cast<int32_t>(id));
        if (e)
        {
            if((e->type == Event::EventType::BURNING_RIDE || e->type == Event::EventType::MIDNIGHT_RIDE) && !save.HasAnyBike())
                continue;

            if (!save.completed_events[static_cast<uint8_t>(e->type)].contains(id))
                open.push_back(std::format("{} - {}", Event::to_string(e->type), e->name));
        }

    }

    if(config.tracker_sort_event_names_alphabetically)
        std::ranges::sort(open);
    return open;
}

std::vector<std::string> GetOpenCarWins() {
    auto& state = App::Instance->State();
    const auto& save  = state.GetSaveData();
    const auto& slot  = state.GetSlotData();
    auto& config = ModConfig::Get();
    std::vector<std::string> open;

    for (auto id : save.obtained_cars)
        if (!save.HasCarWon(id))
        {
            if(const Vehicle::VehicleInfo* v = Vehicle::FindVehicleByID(id))
                open.push_back(std::format("{} - {}", Vehicle::to_string(v->category), v->proper_name));
        }

    if(config.tracker_sort_car_win_names_alphabetically)
        std::ranges::sort(open);
    return open;
}
