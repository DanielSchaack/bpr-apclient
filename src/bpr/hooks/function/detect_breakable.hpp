#pragma once

#include <array>
#include <string_view>

namespace DetectBreakable
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
}
