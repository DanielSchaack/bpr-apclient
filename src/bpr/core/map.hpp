#pragma once

#include <array>
#include <string_view>

namespace Map
{
    inline constexpr std::array<std::string_view, 6> areaIndex = {
        "Palm Bay Heights",
        "Silver Lake",
        "Harbor Town",
        "White Mountain",
        "Downtown Paradise",
        "Big Surf Island"
    };

    inline constexpr std::array<std::string_view, 3> typeIndex = {
        "Super Jump",
        "Smash",
        "Billboard"
    };

    enum class AreaType : uint8_t {
        PALM_BAY_HEIGHTS    = 0,
        SILVER_LAKE         = 1,
        WHITE_MOUNTAIN      = 3,
        HARBOR_TOWN         = 2,
        DOWNTOWN_PARADISE   = 4,
        BIG_SURF_ISLAND     = 5
    };

    [[nodiscard]] constexpr int AreaNameToIndex(std::string_view area) noexcept
    {
        if (area == "Palm Bay Heights")   return 0;
        if (area == "Silver Lake")        return 1;
        if (area == "Harbor Town")        return 2;
        if (area == "White Mountain")     return 3;
        if (area == "Downtown Paradise")  return 4;
        if (area == "Big Surf Island")    return 5;
        return -1;
    }

    [[nodiscard]] constexpr std::string_view AreaTypeToName(AreaType area) noexcept {
        switch (area) {
            case AreaType::PALM_BAY_HEIGHTS: return "Palm Bay Heights";
            case AreaType::SILVER_LAKE:     return "Silver Lake";
            case AreaType::WHITE_MOUNTAIN:  return "White Mountain";
            case AreaType::HARBOR_TOWN:     return "Harbor Town";
            case AreaType::DOWNTOWN_PARADISE: return "Downtown Paradise";
            case AreaType::BIG_SURF_ISLAND: return "Big Surf Island";
        }
        return "Unknown";
    }
}
