#pragma once
#include "bpr/core/map.hpp"
#include <cstdint>
#include <string_view>
#include <vector>


enum class EventType : uint8_t {
    BURNING_ROUTE = 0,
    RACE          = 1,
    ROAD_RAGE     = 2,
    STUNT_RUN     = 3,
    MARKED_MAN    = 4
};


[[nodiscard]] constexpr std::string_view to_string(EventType event) noexcept {
    switch (event) {
        case EventType::BURNING_ROUTE: return "Burning Route";
        case EventType::RACE:          return "Race";
        case EventType::ROAD_RAGE:     return "Road Rage";
        case EventType::STUNT_RUN:     return "Stunt Run";
        case EventType::MARKED_MAN:    return "Marked Man";
    }
    return "Unknown";
}

struct EventInfo {
    EventType    type;
    std::string_view name;
    int32_t      location_id;
    Map::AreaType     area;
};

inline constexpr EventInfo event_locations[] = {
    {EventType::BURNING_ROUTE, "Carson Opus",                480846, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::BURNING_ROUTE, "Montgomery GT 2400",         480847, Map::AreaType::SILVER_LAKE},
    {EventType::RACE,          "Save Ferris",                480850, Map::AreaType::SILVER_LAKE},
    {EventType::RACE,          "Go West!",                   480852, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::RACE,          "Curveball",                  480853, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::BURNING_ROUTE, "Kitano Touge Sport",         480854, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::RACE,          "Seeing Stars",               480856, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::RACE,          "Plain Sailing",              480857, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::RACE,          "Deep South",                 480859, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::BURNING_ROUTE, "Hunter Cavalry",             480860, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::ROAD_RAGE,     "Lighthouse Rock",            480861, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::ROAD_RAGE,     "Motor City Mayhem",          480883, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::ROAD_RAGE,     "Oncoming Onslaught",         480884, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::RACE,          "Stealing 1st",               480886, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::STUNT_RUN,     "Hang 10",                    480888, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::BURNING_ROUTE, "Krieger Pioneer",            480890, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::BURNING_ROUTE, "Nakamura SI-7",              480892, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::RACE,          "Horse Power",                480896, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::STUNT_RUN,     "Lighthouse Party",           480897, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::RACE,          "Tunnel Vision",              480898, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::BURNING_ROUTE, "Carson Thunder Custom",      480899, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::RACE,          "The Duel",                   480900, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::BURNING_ROUTE, "Hunter Vegas",               480901, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::BURNING_ROUTE, "Carson Grand Marais",        480905, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::BURNING_ROUTE, "Carson Annihilator",         480973, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::BURNING_ROUTE, "Krieger 616 Sport",          480974, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::MARKED_MAN,    "Steeplechased",              480975, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::ROAD_RAGE,     "River City Rampage",         480976, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::RACE,          "Rat Race",                   480984, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::BURNING_ROUTE, "Rossolini Tempesta",         480998, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::RACE,          "Race To The Summit",         480999, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::MARKED_MAN,    "Emergency 911",              481003, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::RACE,          "Avant Guard",                481004, Map::AreaType::HARBOR_TOWN},
    {EventType::STUNT_RUN,     "Over Construction",          481005, Map::AreaType::HARBOR_TOWN},
    {EventType::MARKED_MAN,    "Run To The Hills",           481011, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::BURNING_ROUTE, "Montgomery Hyperion",        481012, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::ROAD_RAGE,     "Rush Hour",                  481013, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::BURNING_ROUTE, "Watson 25 V16 Revenge",      481014, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::BURNING_ROUTE, "Carson 500 GT",              481015, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::RACE,          "Final Furlong",              481016, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::STUNT_RUN,     "Bravo, Encore!",             481026, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::BURNING_ROUTE, "Krieger Überschall 8",       481027, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::MARKED_MAN,    "Run Like The Wind",          481028, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::ROAD_RAGE,     "Franke Exchange",            481029, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::BURNING_ROUTE, "Hunter Citizen",             481030, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::MARKED_MAN,    "Strike Out",                 481031, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::BURNING_ROUTE, "Hunter Mesquite",            481032, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::RACE,          "Riverside Run",              481033, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::BURNING_ROUTE, "Montgomery Hawker",          481034, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::MARKED_MAN,    "Man O' War",                 481035, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::ROAD_RAGE,     "Angus Wharfare",             481036, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::STUNT_RUN,     "Unconventional",             481037, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::MARKED_MAN,    "Safe Harbor",                481038, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::BURNING_ROUTE, "Nakamura Ikusa GT",          481040, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::RACE,          "Far, Far Away",              481041, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::MARKED_MAN,    "Power Struggle",             481042, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::ROAD_RAGE,     "Central Square-Off",         481043, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::BURNING_ROUTE, "Rossolini LM Classic",       481045, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::RACE,          "Spin City",                  481048, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::STUNT_RUN,     "Elevation",                  481049, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::BURNING_ROUTE, "Carson Inferno Van",         481050, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::MARKED_MAN,    "Club Sandwich",          481052, Map::AreaType::HARBOR_TOWN},
    {EventType::RACE,          "Spaghetti Western",      481054, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::BURNING_ROUTE, "Watson R-Turbo Roadster",481057, Map::AreaType::HARBOR_TOWN},
    {EventType::MARKED_MAN,    "Mayday Mayday",          481059, Map::AreaType::HARBOR_TOWN},
    {EventType::RACE,          "Coast To Coast",          481060, Map::AreaType::HARBOR_TOWN},
    {EventType::BURNING_ROUTE, "Jansen P12",              481061, Map::AreaType::HARBOR_TOWN},
    {EventType::ROAD_RAGE,     "Blockade Run",            481062, Map::AreaType::HARBOR_TOWN},
    {EventType::MARKED_MAN,    "Rescue Me",               481063, Map::AreaType::HARBOR_TOWN},
    {EventType::RACE,          "Catch My Drift",          481064, Map::AreaType::HARBOR_TOWN},
    {EventType::RACE,          "Race For The Plate",      481065, Map::AreaType::HARBOR_TOWN},
    {EventType::STUNT_RUN,     "Express Yourself",        481066, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::RACE,          "Heads Up",                481068, Map::AreaType::HARBOR_TOWN},
    {EventType::BURNING_ROUTE, "Jansen X12",              481069, Map::AreaType::HARBOR_TOWN},
    {EventType::MARKED_MAN,    "Hostile Waters",          481070, Map::AreaType::HARBOR_TOWN},
    {EventType::STUNT_RUN,     "Offroad Parking",         481074, Map::AreaType::HARBOR_TOWN},
    {EventType::RACE,          "Reach For The Stars",     481075, Map::AreaType::HARBOR_TOWN},
    {EventType::RACE,          "Waterway To Go",          481081, Map::AreaType::HARBOR_TOWN},
    {EventType::BURNING_ROUTE, "Hunter Takedown 4x4",     481083, Map::AreaType::HARBOR_TOWN},
    {EventType::RACE,          "Hard Fort",               481084, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::RACE,          "Baseball Battle",         481085, Map::AreaType::SILVER_LAKE},
    {EventType::STUNT_RUN,     "Base Jumper",             481086, Map::AreaType::SILVER_LAKE},
    {EventType::RACE,          "Power Surge",             481092, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::MARKED_MAN,    "Run Home",                481094, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::RACE,          "Demolition Derby",        481103, Map::AreaType::SILVER_LAKE},
    {EventType::BURNING_ROUTE, "Hunter Spur",             481381, Map::AreaType::SILVER_LAKE},
    {EventType::ROAD_RAGE,     "Half Nelson",             481382, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::RACE,          "Going Coastal",           481383, Map::AreaType::SILVER_LAKE},
    {EventType::STUNT_RUN,     "Near The Edge",           481384, Map::AreaType::SILVER_LAKE},
    {EventType::RACE,          "Full Gallop",             481385, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::BURNING_ROUTE, "Hunter Reliable Custom",  481386, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::STUNT_RUN,     "Falling Down",            481387, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::RACE,          "Mano A Mano",             481388, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::STUNT_RUN,     "About Town",              481389, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::RACE,          "Call Of The Wild",        481393, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::RACE,          "Driving Off",             481394, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::RACE,          "Eastern Promise",         481395, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::ROAD_RAGE,     "Sunset Showdown",         481396, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::RACE,          "Torpedo Run",             481398, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::RACE,          "High Noon Club",          481399, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::STUNT_RUN,     "Cliffhanger",             481400, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::RACE,          "Plaza Endurance",         481401, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::BURNING_ROUTE, "Carson Hot Rod Coupe",    481402, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::BURNING_ROUTE, "Hunter Manhattan",        481424, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::MARKED_MAN,    "Stampede",                481425, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::ROAD_RAGE,     "Cross-town Carnage",      481436, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::RACE,          "Pleasure Cruise",         481438, Map::AreaType::HARBOR_TOWN},
    {EventType::BURNING_ROUTE, "Carson Fastback",         481470, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::STUNT_RUN,     "Rack 'Em Up",             481528, Map::AreaType::SILVER_LAKE},
    {EventType::RACE,          "Lakeside Getaway",        481535, Map::AreaType::SILVER_LAKE},
    {EventType::BURNING_ROUTE, "Carson GT Concept",       481548, Map::AreaType::WHITE_MOUNTAIN},
    {EventType::RACE,          "Rollercoaster",           483041, Map::AreaType::HARBOR_TOWN},
    {EventType::ROAD_RAGE,     "Suburban Scrap",          483052, Map::AreaType::HARBOR_TOWN},
    {EventType::BURNING_ROUTE, "Kitano Hydros Custom",    486462, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::BURNING_ROUTE, "Krieger Racing WTR",      527090, Map::AreaType::PALM_BAY_HEIGHTS},
    {EventType::BURNING_ROUTE, "Hunter Racing Oval Champ",527091, Map::AreaType::HARBOR_TOWN},
    {EventType::ROAD_RAGE,     "Wrecking Yard",           527092, Map::AreaType::HARBOR_TOWN},
    {EventType::ROAD_RAGE,     "Taking Its Toll",         527093, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::MARKED_MAN,    "Press Ganged",            527094, Map::AreaType::DOWNTOWN_PARADISE},
    {EventType::ROAD_RAGE,     "Freeway Frenzy",          533905, Map::AreaType::HARBOR_TOWN},
};

inline const EventInfo* FindByEventId(int32_t location_id)
{
    for (const EventInfo& loc : event_locations)
    {
        if (loc.location_id == location_id)
        {
            return &loc;
        }
    }
    return nullptr;
}

inline std::vector<const EventInfo*> FindAllByEventType(EventType type)
{
    std::vector<const EventInfo*> result;
    for (const EventInfo& loc : event_locations)
    {
        if (loc.type == type)
        {
            result.push_back(&loc);
        }
    }
    return result;
}

inline std::vector<const EventInfo*> FindAllByAreaType(Map::AreaType area)
{
    std::vector<const EventInfo*> result;
    for (const EventInfo& loc : event_locations)
    {
        if (loc.area == area)
        {
            result.push_back(&loc);
        }
    }
    return result;
}
