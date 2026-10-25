#pragma once

#include <array>
#include <string_view>
#include <vector>

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

    inline constexpr std::array<std::string_view, 8> typeIndex = {
        "Super Jump",
        "Smash",
        "Billboard",
        "Drive-Thru",
        "Road Rules for Time",
        "Road Rules for Showtime",
        "Road Rules for Bikes at Day",
        "Road Rules for Bikes at Night"
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


    constexpr AreaType A = AreaType::BIG_SURF_ISLAND;
 
    enum class DriveThruType : uint8_t {
        JUNKYARD        = 0,
        GAS_STATION     = 2,
        AUTO_REPAIR     = 3,
        PAINT_SHOP      = 4,
        PARKING_GARAGE  = 5
    };

    struct DriveThruInfo {
        const char *junction;
        uint64_t id;
        DriveThruType type;
        AreaType area;
        float Position[4];
    };

    inline constexpr DriveThruInfo driveThruInfos[] = {
        { "Angus & E. Crawford",        250700, DriveThruType::JUNKYARD, AreaType::DOWNTOWN_PARADISE, {3022.2f, -3.0f, -1952.5f, 1.0f} },
        { "Manners & S. Rouse",         312262, DriveThruType::JUNKYARD, AreaType::HARBOR_TOWN, {-332.2f, 9.4f, 946.2f, 1.0f} },
        { "Hamilton & Young",           292521, DriveThruType::JUNKYARD, AreaType::PALM_BAY_HEIGHTS, {1265.2f, 14.6f, -510.2f, 1.0f} },
        { "Ross & Nelson",              290890, DriveThruType::JUNKYARD, AreaType::SILVER_LAKE, {-1049.5f, 113.1f, -1863.8f, 1.0f} },
        { "Chubb Lane",                 289762, DriveThruType::JUNKYARD, AreaType::WHITE_MOUNTAIN, {-2195.3f, 102.6f, 389.0f, 1.0f} },
        { "Grange Hill (Big Surf)",     790285, DriveThruType::JUNKYARD, AreaType::BIG_SURF_ISLAND, {4696.3f, 61.2f, -475.9f, 1.0f} },
 
        { "Angus Wharf",                285279, DriveThruType::GAS_STATION, AreaType::DOWNTOWN_PARADISE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Angus & Webster",            243542, DriveThruType::GAS_STATION, AreaType::DOWNTOWN_PARADISE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Lambert & Harber",           297494, DriveThruType::GAS_STATION, AreaType::HARBOR_TOWN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "S. Bay Expwy",               312264, DriveThruType::GAS_STATION, AreaType::HARBOR_TOWN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Andersen St",                230926, DriveThruType::GAS_STATION, AreaType::DOWNTOWN_PARADISE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "East Crawford & Nakamura",   221845, DriveThruType::GAS_STATION, AreaType::PALM_BAY_HEIGHTS, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Hamilton & Young",           228543, DriveThruType::GAS_STATION, AreaType::PALM_BAY_HEIGHTS, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Lambert & S. Rouse",         298635, DriveThruType::GAS_STATION, AreaType::HARBOR_TOWN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "E. Lake & Hubbard",          292537, DriveThruType::GAS_STATION, AreaType::SILVER_LAKE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "E. Lake & Rack",             320557, DriveThruType::GAS_STATION, AreaType::SILVER_LAKE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Nelson Way",                 289804, DriveThruType::GAS_STATION, AreaType::SILVER_LAKE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Nelson & Uphill",            320555, DriveThruType::GAS_STATION, AreaType::WHITE_MOUNTAIN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Chubb Lane",                 290242, DriveThruType::GAS_STATION, AreaType::WHITE_MOUNTAIN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "S. Mountain Dr",             320560, DriveThruType::GAS_STATION, AreaType::WHITE_MOUNTAIN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Shires Dr (Big Surf)",       790292, DriveThruType::GAS_STATION, AreaType::BIG_SURF_ISLAND, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Maguire Rd (Big Surf)",      790291, DriveThruType::GAS_STATION, AreaType::BIG_SURF_ISLAND, {0.0f, 0.0f, 0.0f, 0.0f} },
 
        { "Angus Wharf",                444205, DriveThruType::AUTO_REPAIR, AreaType::DOWNTOWN_PARADISE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "S. Bay Expwy & S. Rouse",    297609, DriveThruType::AUTO_REPAIR, AreaType::HARBOR_TOWN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Harber & Root",              245759, DriveThruType::AUTO_REPAIR, AreaType::DOWNTOWN_PARADISE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "E. Crawford & Hudson",       232041, DriveThruType::AUTO_REPAIR, AreaType::PALM_BAY_HEIGHTS, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Hubbard & Lambert",          252645, DriveThruType::AUTO_REPAIR, AreaType::PALM_BAY_HEIGHTS, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Young Avenue",               219159, DriveThruType::AUTO_REPAIR, AreaType::PALM_BAY_HEIGHTS, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Warren Avenue",              297546, DriveThruType::AUTO_REPAIR, AreaType::HARBOR_TOWN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "S. Rouse",                   308704, DriveThruType::AUTO_REPAIR, AreaType::SILVER_LAKE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Nelson Way",                 294327, DriveThruType::AUTO_REPAIR, AreaType::SILVER_LAKE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "W. Lake & Chubb",            325044, DriveThruType::AUTO_REPAIR, AreaType::WHITE_MOUNTAIN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Schembri & Chubb",           319632, DriveThruType::AUTO_REPAIR, AreaType::WHITE_MOUNTAIN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Pitt Av (Big Surf)",         751303, DriveThruType::AUTO_REPAIR, AreaType::BIG_SURF_ISLAND, {0.0f, 0.0f, 0.0f, 0.0f} },
 
        { "7th & Webster",              241694, DriveThruType::PAINT_SHOP, AreaType::DOWNTOWN_PARADISE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Hamilton & 5th",             227707, DriveThruType::PAINT_SHOP, AreaType::PALM_BAY_HEIGHTS, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Harber & Hamilton",          317170, DriveThruType::PAINT_SHOP, AreaType::HARBOR_TOWN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "E. Lake Drive",              311735, DriveThruType::PAINT_SHOP, AreaType::SILVER_LAKE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "W. Lake & Cannon",           320559, DriveThruType::PAINT_SHOP, AreaType::WHITE_MOUNTAIN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Paradise Keys Brg (Big Surf)", 790452, DriveThruType::PAINT_SHOP, AreaType::BIG_SURF_ISLAND, {0.0f, 0.0f, 0.0f, 0.0f} },
 
        { "Webster Av",                 595524, DriveThruType::PARKING_GARAGE, AreaType::DOWNTOWN_PARADISE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Hudson Av",                  595521, DriveThruType::PARKING_GARAGE, AreaType::DOWNTOWN_PARADISE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Lambert Pkwy (north)",       595522, DriveThruType::PARKING_GARAGE, AreaType::PALM_BAY_HEIGHTS, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Young Av",                   595530, DriveThruType::PARKING_GARAGE, AreaType::PALM_BAY_HEIGHTS, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Hamilton Av",                595525, DriveThruType::PARKING_GARAGE, AreaType::PALM_BAY_HEIGHTS, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Lambert Pkwy (south)",       595528, DriveThruType::PARKING_GARAGE, AreaType::HARBOR_TOWN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Hall Av",                    595527, DriveThruType::PARKING_GARAGE, AreaType::HARBOR_TOWN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Manners Av",                 595526, DriveThruType::PARKING_GARAGE, AreaType::HARBOR_TOWN, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "1st St",                     595523, DriveThruType::PARKING_GARAGE, AreaType::DOWNTOWN_PARADISE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Fry Av",                     595520, DriveThruType::PARKING_GARAGE, AreaType::DOWNTOWN_PARADISE, {0.0f, 0.0f, 0.0f, 0.0f} },
        { "Angus Wharf",                595529, DriveThruType::PARKING_GARAGE, AreaType::DOWNTOWN_PARADISE, {0.0f, 0.0f, 0.0f, 0.0f} },
    };

    struct RoadInfo {
        const char *junction;
        uint64_t id;
        AreaType area;
    };

    inline constexpr RoadInfo roadInfos[] = {
        { "1st St",           8,  AreaType::DOWNTOWN_PARADISE },
        { "2nd St",           4,  AreaType::PALM_BAY_HEIGHTS },
        { "3rd St",           3,  AreaType::PALM_BAY_HEIGHTS },
        { "4th St",           2,  AreaType::DOWNTOWN_PARADISE },
        { "5th St",           9,  AreaType::PALM_BAY_HEIGHTS },
        { "7th St",           35, AreaType::PALM_BAY_HEIGHTS },
        { "9th St",           36, AreaType::PALM_BAY_HEIGHTS },
        { "Andersen St",      55, AreaType::DOWNTOWN_PARADISE },
        { "Angus Wharf",      0,  AreaType::DOWNTOWN_PARADISE },
        { "Cannon Pass",      58, AreaType::WHITE_MOUNTAIN },
        { "Casey Pass",       60, AreaType::WHITE_MOUNTAIN },
        { "Chubb Ln",         24, AreaType::WHITE_MOUNTAIN },
        { "E. Crawford Dr",   53, AreaType::PALM_BAY_HEIGHTS },
        { "E. Lake Dr",       63, AreaType::SILVER_LAKE },
        { "Evans Blvd",       56, AreaType::PALM_BAY_HEIGHTS },
        { "Franke Av",        17, AreaType::DOWNTOWN_PARADISE },
        { "Fry Av",           10, AreaType::DOWNTOWN_PARADISE },
        { "Gabriel Av",       12, AreaType::HARBOR_TOWN },
        { "Geldard Dr",       61, AreaType::HARBOR_TOWN },
        { "Glancey Av",       6,  AreaType::DOWNTOWN_PARADISE },
        { "Hall Av",          34, AreaType::HARBOR_TOWN },
        { "Hamilton Av",      18, AreaType::PALM_BAY_HEIGHTS },
        { "Hans Way",         30, AreaType::WHITE_MOUNTAIN },
        { "Harber St",        50, AreaType::HARBOR_TOWN },
        { "Hawley Av",        39, AreaType::PALM_BAY_HEIGHTS },
        { "Hubbard Av",       45, AreaType::HARBOR_TOWN },
        { "Hudson Av",        42, AreaType::PALM_BAY_HEIGHTS },
        { "I-88 Section 1",   52, AreaType::DOWNTOWN_PARADISE },
        { "I-88 Section 2",   54, AreaType::PALM_BAY_HEIGHTS },
        { "I-88 Section 3",   48, AreaType::SILVER_LAKE },
        { "I-88 Section 4",   49, AreaType::HARBOR_TOWN },
        { "King Av",          16, AreaType::DOWNTOWN_PARADISE },
        { "Lambert Pkwy",     43, AreaType::HARBOR_TOWN },
        { "Lawrence Rd",      59, AreaType::SILVER_LAKE },
        { "Lewis Pass",       20, AreaType::SILVER_LAKE },
        { "Lucas Way",        31, AreaType::WHITE_MOUNTAIN },
        { "Manners Av",       51, AreaType::HARBOR_TOWN },
        { "Moore Av",         41, AreaType::PALM_BAY_HEIGHTS },
        { "N. Mountain Dr",   26, AreaType::WHITE_MOUNTAIN },
        { "N. Rouse Dr",      21, AreaType::SILVER_LAKE },
        { "Nakamura Av",      37, AreaType::PALM_BAY_HEIGHTS },
        { "Nelson Way",       22, AreaType::SILVER_LAKE },
        { "Newton Dr",        33, AreaType::PALM_BAY_HEIGHTS },
        { "Paradise Av",      11, AreaType::PALM_BAY_HEIGHTS },
        { "Parr Av",          44, AreaType::HARBOR_TOWN },
        { "Patterson Av",     5,  AreaType::PALM_BAY_HEIGHTS },
        { "Rack Way",         27, AreaType::SILVER_LAKE },
        { "Read Ln",          19, AreaType::WHITE_MOUNTAIN },
        { "Riverside Av",     7,  AreaType::PALM_BAY_HEIGHTS },
        { "Root Av",          57, AreaType::DOWNTOWN_PARADISE },
        { "Ross Dr",          25, AreaType::SILVER_LAKE },
        { "S. Mountain Dr",   29, AreaType::WHITE_MOUNTAIN },
        { "S. Rouse Dr",      32, AreaType::HARBOR_TOWN },
        { "Schembri Pass",    28, AreaType::WHITE_MOUNTAIN },
        { "Shepherd Av",      13, AreaType::HARBOR_TOWN },
        { "South Bay Expwy",  47, AreaType::HARBOR_TOWN },
        { "Sullivan Av",      38, AreaType::PALM_BAY_HEIGHTS },
        { "Uphill Dr",        23, AreaType::WHITE_MOUNTAIN },
        { "W. Crawford Dr",   40, AreaType::SILVER_LAKE },
        { "W. Lake Dr",       62, AreaType::WHITE_MOUNTAIN },
        { "Warren Av",        14, AreaType::HARBOR_TOWN },
        { "Watt St",          1,  AreaType::DOWNTOWN_PARADISE },
        { "Webster Av",       46, AreaType::DOWNTOWN_PARADISE },
        { "Young Av",         15, AreaType::PALM_BAY_HEIGHTS },

    { "Glover Way",        64, AreaType::BIG_SURF_ISLAND },
    { "Grange Hill",       65, AreaType::BIG_SURF_ISLAND },
    { "Huntley Blvd",      66, AreaType::BIG_SURF_ISLAND },
    { "Griffin Blvd",      67, AreaType::BIG_SURF_ISLAND },
    { "Jacobs St",         68, AreaType::BIG_SURF_ISLAND },
    { "Lipscomb Way",      69, AreaType::BIG_SURF_ISLAND },
    { "Paradise Keys Brg", 70, AreaType::BIG_SURF_ISLAND },
    { "Daniell Way",       71, AreaType::BIG_SURF_ISLAND },
    { "Pitt Av",           72, AreaType::BIG_SURF_ISLAND },
    { "McDonald Rd",       73, AreaType::BIG_SURF_ISLAND },
    { "Shires Dr",         74, AreaType::BIG_SURF_ISLAND },
    { "Maguire Rd",        75, AreaType::BIG_SURF_ISLAND },
    };

    inline const DriveThruInfo* FindDriveThruByID(uint64_t drivethruId)
    {
        for (const DriveThruInfo& drivethru : driveThruInfos)
        {
            if (drivethru.id == drivethruId)
            {
                return &drivethru;
            }
        }
        return nullptr;
    }

    inline std::vector<const DriveThruInfo*> FindAllDriveThrusByArea(int areaId)
    {
        std::vector<const DriveThruInfo*> result;
        const AreaType area = static_cast<AreaType>(areaId);

        for (const DriveThruInfo& drivethru : driveThruInfos)
        {
            if (drivethru.area == area)
            {
                result.push_back(&drivethru);
            }
        }
        return result;
    }

    inline const RoadInfo* FindRoadInfoByID(uint64_t roadId)
    {
        for (const RoadInfo& road : roadInfos)
        {
            if (road.id == roadId)
            {
                return &road;
            }
        }
        return nullptr;
    }
}
