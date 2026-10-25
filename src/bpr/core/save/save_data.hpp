#pragma once
#include "bpr/core/events.hpp"
#include "bpr/core/logger.hpp"
#include "bpr/core/map.hpp"
#include "bpr/core/vehicles.hpp"
#include <format>
#include <string>
#include <nlohmann/json.hpp>
#include <unordered_set>

namespace bpr
{
    struct SaveData
    {
        std::string seed_;
        int slot_;
        int lastIndex_;
        int license = 0;
        std::array<std::array<int, Map::typeIndex.size()>, Map::areaIndex.size()> breakable_counts{};
        std::array<std::array<bool, Map::typeIndex.size()>, Map::areaIndex.size()> breakable_owned{};
        std::array<std::array<std::vector<int>, Map::typeIndex.size()>, Map::areaIndex.size()> deferred_breakables{};
        std::unordered_set<uint64_t> obtained_paradise_cars{};
        std::unordered_set<uint64_t> obtained_cars{};
        std::unordered_set<uint64_t> obtained_liveries{};
        std::unordered_set<uint64_t> obtained_car_events{};
        std::unordered_set<uint64_t> obtained_bike_events{};
        std::unordered_set<uint64_t> obtained_drivethru_jumppoints{};
        std::unordered_set<uint64_t> visited_drivethrus{};
        std::array<std::unordered_set<uint64_t>, 8> completed_events{};
        std::array<std::unordered_set<uint64_t>, 5> completed_cars{};
        std::unordered_set<uint64_t> completed_unique_cars{};

        bool HasCar(uint64_t carId) const noexcept
        {
            if (obtained_cars.contains(carId))
                return true;

            if (const Vehicle::VehicleInfo* info = Vehicle::FindVehicleByLiveryID(carId))
                return HasLivery(info->archipelago_livery_id);
            return false;
        }

        void AddCar(uint64_t carId)
        {
            Logger::Log(std::format("Adding car {:x}", carId));
            const Vehicle::VehicleInfo* info = Vehicle::FindVehicleByID(carId);
            if (!info)
                return;

            if (obtained_cars.insert(info->id).second)   // true = it was new
            {
                Logger::Log(std::format("Vehicle info found, added car {:x}", info->id));
                if (info->category == Vehicle::VehicleCategory::ParadiseCars)
                    obtained_paradise_cars.insert(info->id);
            }
        }

        bool HasLivery(uint64_t liveryId) const noexcept
        {
            return obtained_liveries.contains(liveryId);
        }

        void AddLivery(uint64_t liveryId)
        {
            obtained_liveries.insert(liveryId);
        }

        void AddEventWin(uint32_t eventId, bool isCar)
        {
            const Event::EventInfo* info = isCar ? Event::FindByEventId(eventId) : Event::FindByBikeEventId(eventId);
            if (!info)
                return;

            int biketype = info->type == Event::EventType::BURNING_RIDE ? 1:0;
            if(!biketype)
                biketype = info->type == Event::EventType::MIDNIGHT_RIDE ? 2:0;

            if (isCar)
                completed_events[static_cast<uint8_t>(info->type)].insert(eventId);
            else if (biketype)
                completed_events[4 + biketype].insert(eventId);
        }

        int GetEventWins(Event::EventType event_type) const noexcept
        {
            return static_cast<int>(completed_events[static_cast<uint8_t>(event_type)].size());
        }

        void AddCarWin(uint64_t carId)
        {
            const Vehicle::VehicleInfo* info = Vehicle::FindVehicleByID(carId);
            if (!info)
                return;

            completed_cars[static_cast<uint8_t>(Vehicle::BoostTypeToIndex(info->boost_type))].insert(carId);
            completed_unique_cars.insert(carId);
        }

        bool HasCarWon(uint64_t carId) const noexcept
        {
            return completed_unique_cars.contains(carId);
        }

        int GetCarWins(Vehicle::BoostType boost_type) const noexcept
        {
            return static_cast<int>(completed_cars[static_cast<uint8_t>(Vehicle::BoostTypeToIndex(boost_type))].size());
        }

        bool HasBikeEvent(uint64_t eventId) const noexcept
        {
            return obtained_bike_events.contains(eventId);
        }
    };


    inline void to_json(nlohmann::json& j, const SaveData& data)
    {
        j = nlohmann::json{
            {"seed", data.seed_},
            {"slot", data.slot_},
            {"lastIndex", data.lastIndex_},
            {"license", data.license},
            {"breakable_counts", data.breakable_counts},
            {"breakable_owned", data.breakable_owned},
            {"deferred_breakables", data.deferred_breakables},
            {"obtained_paradise_cars", data.obtained_paradise_cars},
            {"obtained_cars", data.obtained_cars},
            {"obtained_liveries", data.obtained_liveries},
            {"obtained_car_events", data.obtained_car_events},
            {"obtained_bike_events", data.obtained_bike_events},
            {"obtained_drivethru_jumppoints", data.obtained_drivethru_jumppoints},
            {"visited_drivethrus", data.visited_drivethrus},
            {"completed_events", data.completed_events},
            {"completed_cars", data.completed_cars},
            {"completed_unique_cars", data.completed_unique_cars}
        };
    }

    inline void from_json(const nlohmann::json& j, SaveData& data)
    {
        j.at("seed").get_to(data.seed_);
        j.at("slot").get_to(data.slot_);
        j.at("lastIndex").get_to(data.lastIndex_);
        j.at("license").get_to(data.license);
        j.at("breakable_counts").get_to(data.breakable_counts);
        j.at("breakable_owned").get_to(data.breakable_owned);
        j.at("deferred_breakables").get_to(data.deferred_breakables);
        j.at("obtained_paradise_cars").get_to(data.obtained_paradise_cars);
        j.at("obtained_cars").get_to(data.obtained_cars);
        j.at("obtained_liveries").get_to(data.obtained_liveries);
        j.at("obtained_car_events").get_to(data.obtained_car_events);
        j.at("obtained_bike_events").get_to(data.obtained_bike_events);
        j.at("obtained_drivethru_jumppoints").get_to(data.obtained_drivethru_jumppoints);
        j.at("visited_drivethrus").get_to(data.visited_drivethrus);
        j.at("completed_events").get_to(data.completed_events);
        j.at("completed_cars").get_to(data.completed_cars);
        j.at("completed_unique_cars").get_to(data.completed_unique_cars);
    }
}
