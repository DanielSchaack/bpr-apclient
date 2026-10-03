#pragma once
#include "bpr/core/map.hpp"
#include "bpr/core/vehicles.hpp"
#include <string>
#include <nlohmann/json.hpp>

namespace bpr
{
    struct SaveData
    {
        std::string seed_;
        int slot_;
        int lastIndex_;
        std::array<std::array<int, Map::typeIndex.size()>, Map::areaIndex.size()> breakable_counts{};
        std::array<std::array<bool, Map::typeIndex.size()>, Map::areaIndex.size()> breakable_owned{};
        std::array<std::array<std::vector<int>, Map::typeIndex.size()>, Map::areaIndex.size()> deferred_breakables{};
        std::vector<uint64_t> obtained_cars{};
        std::vector<uint64_t> obtained_liveries{};
        std::vector<uint64_t> completed_events{};

        bool HasCar(uint64_t carId) const noexcept
        {
            for (uint64_t id : obtained_cars)
            {
                if (id == carId) return true;
                const VehicleInfo* info = FindVehicleByLiveryID(carId);
                if (info) return HasLivery(info->archipelago_livery_id);
            }
            return false;
        }

        void AddCar(uint64_t carId)
        {
            if(!HasCar(carId))
                obtained_cars.push_back(carId);
        }

        bool HasLivery(uint64_t liveryId) const noexcept
        {
            for (uint64_t id : obtained_liveries)
            {
                if (id == liveryId) return true;
            }
            return false;
        }

        void AddLivery(uint64_t liveryId)
        {
            if(!HasLivery(liveryId))
                obtained_liveries.push_back(liveryId);
        }
    };


    inline void to_json(nlohmann::json& j, const SaveData& data)
    {
        j = nlohmann::json{
            {"seed", data.seed_},
            {"slot", data.slot_},
            {"lastIndex", data.lastIndex_},
            {"breakable_counts", data.breakable_counts},
            {"breakable_owned", data.breakable_owned},
            {"deferred_breakables", data.deferred_breakables},
            {"obtained_cars", data.obtained_cars},
            {"completed_events", data.completed_events}
        };
    }

    inline void from_json(const nlohmann::json& j, SaveData& data)
    {
        j.at("seed").get_to(data.seed_);
        j.at("slot").get_to(data.slot_);
        j.at("lastIndex").get_to(data.lastIndex_);
        j.at("breakable_counts").get_to(data.breakable_counts);
        j.at("breakable_owned").get_to(data.breakable_owned);
        j.at("deferred_breakables").get_to(data.deferred_breakables);
        j.at("obtained_cars").get_to(data.obtained_cars);
        j.at("completed_events").get_to(data.completed_events);
    }
}
