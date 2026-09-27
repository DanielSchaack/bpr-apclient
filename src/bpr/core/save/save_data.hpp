#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace bpr
{
    struct SaveData
    {
        std::string seed_;
        int slot_;
        int lastIndex_;
        std::array<std::array<int, 3>, 6> breakable_counts{};
        std::array<std::array<bool, 3>, 6> breakable_owned{};
        std::array<std::array<std::vector<int>, 3>, 6> deferred_breakables{};
    };

    inline void to_json(nlohmann::json& j, const SaveData& data)
    {
        j = nlohmann::json{
            {"seed", data.seed_},
            {"slot", data.slot_},
            {"lastIndex", data.lastIndex_},
            {"breakable_counts", data.breakable_counts},
            {"breakable_owned", data.breakable_owned},
            {"deferred_breakables", data.deferred_breakables}
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
    }
}
