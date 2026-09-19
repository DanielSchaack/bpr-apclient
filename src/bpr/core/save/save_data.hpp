#pragma once
#include <cstdint>
#include <list>
#include <map>
#include <string>
#include <nlohmann/json.hpp>

namespace bpr
{
    struct SaveData
    {
        std::string seed_;
        int slot_;
        int lastIndex_;
        std::array<int, 6> breakable_counts = {
            0,0,0,0,0,0
        };
        std::list<int> owned_areas = {};
        std::map<int, std::list<int64_t>> defered_breakables = {};
    };

    inline void to_json(nlohmann::json& j, const SaveData& data)
    {
        j = nlohmann::json{
            {"seed", data.seed_},
            {"slot", data.slot_},
            {"lastIndex", data.lastIndex_},
            {"breakable_counts", data.breakable_counts},
            {"owned_areas", data.owned_areas},
            {"defered_breakables", data.defered_breakables}
        };
    }

    inline void from_json(const nlohmann::json& j, SaveData& data)
    {
        j.at("seed").get_to(data.seed_);
        j.at("slot").get_to(data.slot_);
        j.at("lastIndex").get_to(data.lastIndex_);
        j.at("breakable_counts").get_to(data.breakable_counts);
        j.at("owned_areas").get_to(data.owned_areas);
        j.at("defered_breakables").get_to(data.defered_breakables);
    }
}