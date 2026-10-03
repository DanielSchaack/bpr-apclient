#pragma once

#include <cstdint>
#include <vector>

enum class UnlockType : uint8_t {
    Unlock        = 0,  // Unlocked at start
    Gift          = 1,  // Secondary finishes and Burning Route unlocks
    Trophy        = 2,  // Unlocked through achievements (carbon cars)
    ShutdownRival = 3,
    GoldSilver    = 4,  // Gold and platinum cars
    Sponsor       = 5,  // Will not show until a certain rank is reached
    OnlineOnly    = 6,  // Used on online cars. Causes vehicles to only show while online
    BeatTheTeam   = 7,  // Used on Beat The Team community cars (Skins for Tempesta Dream/Tiger GT)
    DLC           = 8,  // Used on PDLC vehicles
    Cop           = 9,  // Used on Cop Cars
    IslandGift    = 10, // Island gift
    IslandUnlock  = 11  // Island unlock
};

enum class VehicleCategory : uint8_t {
    ParadiseCars        = 0x01,
    ParadiseBikes       = 0x02,
    OnlineCars          = 0x04,
    ToyVehicles         = 0x08,
    LegendaryCars       = 0x10,
    BoostSpecialCars    = 0x20,
    CopCars             = 0x40,
    BigSurfIslandCars   = 0x80,
};

enum class BoostType : uint8_t {
    Speed    = 0,
    Crash    = 1,
    Stunt    = 2,
    Disabled = 3,
    Infinite = 4, // not official
    Multi    = 5  // not official
};


inline const int8_t max_livery_amount = 7;
struct VehicleInfo {
    uint64_t id;
    uint64_t archipelago_car_id;
    uint64_t archipelago_livery_id;
    uint64_t adjacent_ids[max_livery_amount];
    const char* name;
    const char* proper_name;
    UnlockType unlock_type;
    VehicleCategory category;
    uint8_t speed;
    uint8_t boost;
    uint8_t strength;
    BoostType boost_type;
};

// see disable_add_car.cpp
struct CarData
{
    std::uint64_t id;            // +00
    std::uint8_t colour;         // +08
    std::uint8_t palette;        // +09
    std::uint8_t unlockShown;    // +0A
    std::uint8_t category;       // +0B
    float deformation;           // +0C
    std::uint32_t unlockType;    // +10
    std::uint32_t reserved;      // +14; not assigned a semantic name
};

struct CarInfo
{
    std::uint64_t id           = 0;
    std::uint8_t  category     = 0xFF;   // CarData+0x0B of the live record
    std::uint32_t unlockType   = 0;      // CarData+0x10 of the live record
    const VehicleInfo* table   = nullptr;
    bool          isBike       = false;
};

inline bool IsBikeCategory(VehicleCategory c) noexcept
{
    return c == VehicleCategory::ParadiseBikes;
}

inline constexpr VehicleInfo vehicleInfos[] = {
    {
        .id = 0xA7E60F1A3A360000,
        .archipelago_car_id = 0xA7E60F1A3A360,
        .archipelago_livery_id = 2001,
        .adjacent_ids = { 0xA7E60F1B660BD000, 0xA7E60F1B6632E000, 0xA7E60F1A7D598000, 0xA7E60F1A95C38000 },
        .name = "PUSMC01  - CAVALRY",
        .proper_name = "Hunter Cavalry",
        .unlock_type = UnlockType::Unlock,
        .category = VehicleCategory::ParadiseCars,
        .speed = 1,
        .boost = 1,
        .strength = 5,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xD676FB5119E20000,
        .archipelago_car_id = 0xD676FB5119E20,
        .archipelago_livery_id = 2002,
        .adjacent_ids = { 0xD676FB511FFC8000, 0xD676FB5126170000, 0xD676FB515D058000, 0xD676FB51756F8000 },
        .name = "XUSM1B1  - OVAL CHAMP 69",
        .proper_name = "Hunter Oval Champ 69",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 1,
        .boost = 1,
        .strength = 6,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA7E5D4F26592D000,
        .archipelago_car_id = 0xA7E5D4F26592D,
        .archipelago_livery_id = 2003,
        .adjacent_ids = { 0xA7E5D60A0FCFD000, 0xA7E5D60A0FF6E000, 0xA7E5D4F267197000, 0xA7E5D4F267B5B000 },
        .name = "PUSCLT02 - MESQUITE",
        .proper_name = "Hunter Mesquite",
        .unlock_type = UnlockType::Sponsor,
        .category = VehicleCategory::ParadiseCars,
        .speed = 1,
        .boost = 2,
        .strength = 6,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xD676F97EFEC34000,
        .archipelago_car_id = 0xD676F97EFEC34,
        .archipelago_livery_id = 2004,
        .adjacent_ids = { 0xD676C3DA0FCFD000, 0xD676C3DA0FF6E000, 0xD676F97F0070F000, 0xD676F97F010D3000 },
        .name = "XUSLT2B1 - MESQUITE CUSTOM",
        .proper_name = "Hunter Mesquite Custom",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 1,
        .boost = 2,
        .strength = 6,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA4FCC11A5567C000,
        .archipelago_car_id = 0xA4FCC11A5567C,
        .archipelago_livery_id = 2005,
        .adjacent_ids = { 0xA4FCC11A9297D000, 0xA4FCC11A92BEE000, 0xA4FCC11A57157000, 0xA4FCC11A57B1B000 },
        .name = "PASBSC01 - SI-7",
        .proper_name = "Nakamura SI-7",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 1,
        .boost = 2,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xD38DAEEA988B4000,
        .archipelago_car_id = 0xD38DAEEA988B4,
        .archipelago_livery_id = 2006,
        .adjacent_ids = { 0xD38DAEEA98B25000, 0xD38DAEEA98D96000, 0xD38DAEEA9A38F000, 0xD38DAEEA9AD53000 },
        .name = "XASBSCB1  - RACING SI-7",
        .proper_name = "Nakamura Racing SI-7",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 1,
        .boost = 2,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA7E5D37F70360000,
        .archipelago_car_id = 0xA7E5D37F70360,
        .archipelago_livery_id = 2007,
        .adjacent_ids = { 0xA7E5D388FFB88000, 0xA7E5D38905D30000, 0xA7E5D37FB3598000, 0xA7E5D37FCBC38000 },
        .name = "PUSCC01  - VEGAS",
        .proper_name = "Hunter Vegas",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 1,
        .boost = 2,
        .strength = 6,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xD676C159EDC20000,
        .archipelago_car_id = 0xD676C159EDC20,
        .archipelago_livery_id = 2008,
        .adjacent_ids = { 0xD676C159F3DC8000, 0xD676C159F9F70000, 0xD676C15A30E58000, 0xD676C15A494F8000 },
        .name = "XUSCCB1  - VEGAS CARNIVALE",
        .proper_name = "Hunter Vegas Carnivale",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 1,
        .boost = 3,
        .strength = 6,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA59406A49B160000,
        .archipelago_car_id = 0xA59406A49B160,
        .archipelago_livery_id = 2009,
        .adjacent_ids = { 0xA59406AE2A988000, 0xA59406AE30B30000, 0xA59406A4DE398000, 0xA59406A4F6A38000 },
        .name = "PEUSV01  - PIONEER",
        .proper_name = "Krieger Pioneer",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 1,
        .boost = 2,
        .strength = 9,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xD424F47F18A20000,
        .archipelago_car_id = 0xD424F47F18A20,
        .archipelago_livery_id = 2010,
        .adjacent_ids = { 0xD424F47F1EBC8000, 0xD424F47F24D70000, 0xD424F47F5BC58000, 0xD424F47F742F8000 },
        .name = "XEUSVB1  - PIONEER SUPER GATOR",
        .proper_name = "Krieger Pioneer Super Gator",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 1,
        .boost = 3,
        .strength = 9,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA4FCC10EE9360000,
        .archipelago_car_id = 0xA4FCC10EE9360,
        .archipelago_livery_id = 2011,
        .adjacent_ids = { 0xA4FCC110150BD000, 0xA4FCC1101532E000, 0xA4FCC10F2C598000, 0xA4FCC10F44C38000 },
        .name = "PASBS01  - IKUSA GT",
        .proper_name = "Nakamura Ikusa GT",
        .unlock_type = UnlockType::Sponsor,
        .category = VehicleCategory::ParadiseCars,
        .speed = 2,
        .boost = 3,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xD38DAEE966C20000,
        .archipelago_car_id = 0xD38DAEE966C20,
        .archipelago_livery_id = 2012,
        .adjacent_ids = { 0xD38DAEE01B265000, 0xD38DAEE01B4D6000, 0xD38DAEE9A9E58000, 0xD38DAEE9C24F8000 },
        .name = "XASBSB1  - IKUSA SAMURAI",
        .proper_name = "Nakamura Ikusa Samurai",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 2,
        .boost = 3,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0x59504DAA96298000,
        .archipelago_car_id = 0x59504DAA96298,
        .archipelago_livery_id = 2013,
        .adjacent_ids = {},
        .name = "CARBB1GT  - CARBON IKUSA GT",
        .proper_name = "Nakamura Carbon Ikusa GT",
        .unlock_type = UnlockType::Trophy,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 7,
        .strength = 5,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA4FCBEB7FB67C000,
        .archipelago_car_id = 0xA4FCBEB7FB67C,
        .archipelago_livery_id = 2014,
        .adjacent_ids = { 0xA4FCBEB83897D000, 0xA4FCBEB838BEE000, 0xA4FCBEB7FD157000, 0xA4FCBEB7FDB1B000 },
        .name = "PASBCC01  - HYDROS CUSTOM",
        .proper_name = "Kitano Hydros Custom",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 2,
        .boost = 3,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xD38DAC870CC20000,
        .archipelago_car_id = 0xD38DAC870CC20,
        .archipelago_livery_id = 2015,
        .adjacent_ids = { 0xD38DAC8712DC8000, 0xD38DAC8718F70000, 0xD38DAC874FE58000, 0xD38DAC87684F8000 },
        .name = "XASBCB1  - HYDROS TECHNO",
        .proper_name = "Kitano Hydros Techno",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 2,
        .boost = 3,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0x59504DAB6F4B7000,
        .archipelago_car_id = 0x59504DAB6F4B7,
        .archipelago_livery_id = 2016,
        .adjacent_ids = {},
        .name = "CARBB2CC  - CARBON HYDROS CUSTOM",
        .proper_name = "Kitano Carbon Hydros Custom",
        .unlock_type = UnlockType::Trophy,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 7,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA7E5D607EFD60000,
        .archipelago_car_id = 0xA7E5D607EFD60,
        .archipelago_livery_id = 2017,
        .adjacent_ids = { 0xA7E5D6091BABD000, 0xA7E5D6091BD2E000, 0xA7E5D60832F98000, 0xA7E5D6084B638000 },
        .name = "PUSCT01  - RELIABLE CUSTOM",
        .proper_name = "Hunter Reliable Custom",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 2,
        .boost = 3,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xD676C3D7F1F8E000,
        .archipelago_car_id = 0xD676C3D7F1F8E,
        .archipelago_livery_id = 2018,
        .adjacent_ids = { 0xD676C3D921C65000, 0xD676C3D921ED6000, 0xD676C3D7F21FF000, 0xD676C3D7F2BC3000 },
        .name = "XUSCT01B  - RELIABLE SPECIAL",
        .proper_name = "Hunter Reliable Special",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 2,
        .boost = 3,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA593A0B813960000,
        .archipelago_car_id = 0xA593A0B813960,
        .archipelago_livery_id = 2019,
        .adjacent_ids = { 0xA593A0C1A3188000, 0xA593A0C1A9330000, 0xA593A0B856B98000, 0xA593A0B86F238000 },
        .name = "PEUBR01  - R-TURBO ROADSTER",
        .proper_name = "Watson R-Turbo Roadster",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 2,
        .boost = 3,
        .strength = 3,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xD4248E9278B80000,
        .archipelago_car_id = 0xD4248E9278B80,
        .archipelago_livery_id = 2020,
        .adjacent_ids = { 0xD4248E92973C8000, 0xD4248E929D570000, 0xD4248E936CDC0000, 0xD4248E973D6C0000 },
        .name = "XEUBRB   - BURNOUT ROADSTER",
        .proper_name = "Watson Burnout Roadster",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 2,
        .boost = 4,
        .strength = 3,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA593DB9421760000,
        .archipelago_car_id = 0xA593DB9421760,
        .archipelago_livery_id = 2021,
        .adjacent_ids = { 0xA593DB9DB0F88000, 0xA593DB9DB7130000, 0xA593DB9464998000, 0xA593DB947D038000 },
        .name = "PEULM01  - LM CLASSIC",
        .proper_name = "Rossolini LM Classic",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 2,
        .boost = 4,
        .strength = 2,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xD424C965533F4000,
        .archipelago_car_id = 0xD424C965533F4,
        .archipelago_livery_id = 2022,
        .adjacent_ids = { 0xD424C96EA51C8000, 0xD424C96EAB370000, 0xD424C96554ECF000, 0xD424C96555893000 },
        .name = "XEULM1B1  - LM TRACK PACKAGE",
        .proper_name = "Rossolini LM Track Package",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 2,
        .boost = 4,
        .strength = 2,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA7E5CD898F360000,
        .archipelago_car_id = 0xA7E5CD898F360,
        .archipelago_livery_id = 2023,
        .adjacent_ids = { 0xA7E5CD931EB88000, 0xA7E5CD9324D30000, 0xA7E5CD89D2598000, 0xA7E5CD89EAC38000 },
        .name = "PUSBC01  - MANHATTAN",
        .proper_name = "Hunter Manhattan",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 3,
        .boost = 4,
        .strength = 7,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xD676BB640CC20000,
        .archipelago_car_id = 0xD676BB640CC20,
        .archipelago_livery_id = 2024,
        .adjacent_ids = { 0xD676BB6412DC8000, 0xD676BB6418F70000, 0xD676BB644FE58000, 0xD676BB64684F8000 },
        .name = "XUSBCB1  - MANHATTAN CUSTOM",
        .proper_name = "Hunter Manhattan Custom",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 4,
        .strength = 7,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA7E60D533AB80000,
        .archipelago_car_id = 0xA7E60D533AB80,
        .archipelago_livery_id = 2025,
        .adjacent_ids = { 0xA7E60DCC77188000, 0xA7E60DCC7D330000, 0xA7E60D5BCFFC0000, 0xA7E60D5FA08C0000 },
        .name = "PUSM03   - FASTBACK",
        .proper_name = "Carson Fastback",
        .unlock_type = UnlockType::Sponsor,
        .category = VehicleCategory::ParadiseCars,
        .speed = 3,
        .boost = 4,
        .strength = 5,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xD676FF9BF8EB0000,
        .archipelago_car_id = 0xD676FF9BF8EB0,
        .archipelago_livery_id = 2026,
        .adjacent_ids = { 0xD676FB9D70BAC000, 0xD676FB9D76D54000, 0xD676FB9E40DC0000, 0xD676FBA2116C0000 },
        .name = "XUSMU3B  - FASTBACK SPECIAL",
        .proper_name = "Carson Fastback Special",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 3,
        .boost = 5,
        .strength = 5,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA7E5EB0AA8F60000,
        .archipelago_car_id = 0xA7E5EB0AA8F60,
        .archipelago_livery_id = 2027,
        .adjacent_ids = { 0xA7E5EB0BD4CBD000, 0xA7E5EB0BD4F2E000, 0xA7E5EB0AEC198000, 0xA7E5EB0B04838000 },
        .name = "PUSGA01  - GRAND MARAIS",
        .proper_name = "Carson Grand Marais",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 3,
        .boost = 5,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA7E5EB1526820000,
        .archipelago_car_id = 0xA7E5EB1526820,
        .archipelago_livery_id = 2028,
        .adjacent_ids = { 0xA7E5EB152C9C8000, 0xA7E5EB1532B70000, 0xA7E5EB1569A58000, 0xA7E5EB15820F8000 },
        .name = "PUSGAB1  - GRAND SICILIAN",
        .proper_name = "Carson Grand Sicilian",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 3,
        .boost = 5,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA594060C04960000,
        .archipelago_car_id = 0xA594060C04960,
        .archipelago_livery_id = 2029,
        .adjacent_ids = { 0xA594061594188000, 0xA59406159A330000, 0xA594060C47B98000, 0xA594060C60238000 },
        .name = "PEUSR01  - HYPERION",
        .proper_name = "Montgomery Hyperion",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 3,
        .boost = 4,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xD424F3E682220000,
        .archipelago_car_id = 0xD424F3E682220,
        .archipelago_livery_id = 2030,
        .adjacent_ids = { 0xD424F3E6883C8000, 0xD424F3E68E570000, 0xD424F3E6C5458000, 0xD424F3E6DDAF8000 },
        .name = "XEUSRB1  - HYPERION RATTLER",
        .proper_name = "Montgomery Hyperion Rattler",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 3,
        .boost = 5,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA59403CFDC6B0000,
        .archipelago_car_id = 0xA59403CFDC6B0,
        .archipelago_livery_id = 2031,
        .adjacent_ids = { 0xA59402820D188000, 0xA594028213330000, 0xA59403D013598000, 0xA59403D02BC38000 },
        .name = "PEUSC03  - 616 SPORT",
        .proper_name = "Krieger 616 Sport",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 3,
        .boost = 5,
        .strength = 5,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xD424F1A2EA474000,
        .archipelago_car_id = 0xD424F1A2EA474,
        .archipelago_livery_id = 2032,
        .adjacent_ids = { 0xD424F1A2EA6E5000, 0xD424F1A2EA956000, 0xD424F1A2EBF4F000, 0xD424F1A2EC913000 },
        .name = "XEUSC3B1  - 616 ARACHNO SPORT",
        .proper_name = "Krieger 616 Arachno Sport",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 5,
        .strength = 5,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA7E60F6685760000,
        .archipelago_car_id = 0xA7E60F6685760,
        .archipelago_livery_id = 2033,
        .adjacent_ids = { 0xA7E60F7014F88000, 0xA7E60F701B130000, 0xA7E60F66C8998000, 0xA7E60F66E1038000 },
        .name = "PUSME01  - SPUR",
        .proper_name = "Hunter Spur",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 5,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xD676FD4103020000,
        .archipelago_car_id = 0xD676FD4103020,
        .archipelago_livery_id = 2034,
        .adjacent_ids = { 0xD676FD41091C8000, 0xD676FD410F370000, 0xD676FD4146258000, 0xD676FD415E8F8000 },
        .name = "XUSMEB1  - HOTSPUR",
        .proper_name = "Hunter Hotspur",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 5,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA593FDD9FB6B0000,
        .archipelago_car_id = 0xA593FDD9FB6B0,
        .archipelago_livery_id = 2035,
        .adjacent_ids = { 0xA593FC8C2C188000, 0xA593FC8C32330000, 0xA593FDDA32598000, 0xA593FDDA4AC38000 },
        .name = "PEURC03  - GT 2400",
        .proper_name = "Montgomery GT 2400",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 6,
        .strength = 2,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xD424EBAD09474000,
        .archipelago_car_id = 0xD424EBAD09474,
        .archipelago_livery_id = 2036,
        .adjacent_ids = { 0xD424EA5D203C8000, 0xD424EA5D26570000, 0xD424EBAD0AF4F000, 0xD424EBAD0B913000 },
        .name = "XEURC3B1  - SABOTAGE GT 2400",
        .proper_name = "Montgomery Sabotage GT 2400",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 6,
        .strength = 2,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA7E632DD80360000,
        .archipelago_car_id = 0xA7E632DD80360,
        .archipelago_livery_id = 2037,
        .adjacent_ids = { 0xD67720B70FB88000, 0xA7E632E715D30000, 0xA7E632DDC3598000, 0xA7E632DDDBC38000 },
        .name = "PUSSC01  - P12",
        .proper_name = "Jansen P12",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 6,
        .strength = 3,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xD67720B7FDC20000,
        .archipelago_car_id = 0xD67720B7FDC20,
        .archipelago_livery_id = 2038,
        .adjacent_ids = { 0xD67720B803DC8000, 0xD67720B809F70000, 0xD67720B840E58000, 0xD67720B8594F8000 },
        .name = "XUSSCB1  - P12 TRACK PACKAGE",
        .proper_name = "Jansen P12 Track Package",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 6,
        .strength = 3,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA7E5D6543B160000,
        .archipelago_car_id = 0xA7E5D6543B160,
        .archipelago_livery_id = 2039,
        .adjacent_ids = { 0xA7E5D65566EBD000, 0xA7E5D6556712E000, 0xA566020D0000D000, 0xA566020D23675000, 0xA566020D238E6000, 0xA7E5D6547E398000, 0xA7E5D65496A38000 },
        .name = "PUSCV01  - INFERNO VAN",
        .proper_name = "Carson Inferno Van",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 6,
        .strength = 9,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xD676C4256CDF4000,
        .archipelago_car_id = 0xD676C4256CDF4,
        .archipelago_livery_id = 2040,
        .adjacent_ids = { 0xD676C4256D065000, 0xD676C4256D2D6000, 0xD676C4256E8CF000, 0xD676C4256F293000 },
        .name = "XUSCV1B1  - INFERNO BRT VAN",
        .proper_name = "Carson Inferno BRT Van",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 6,
        .strength = 8,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA59403CFD6508000,
        .archipelago_car_id = 0xA59403CFD6508,
        .archipelago_livery_id = 2041,
        .adjacent_ids = { 0xA594025BE7788000, 0xA594025BED930000, 0xA5234FBC86D60000, 0xA5234FBCC3DF0000, 0xA5234FBCC9F98000, 0xA59403CFD89A7000, 0xA59403CFD936B000 },
        .name = "PEUSC02  - TEMPESTA",
        .proper_name = "Rossolini Tempesta",
        .unlock_type = UnlockType::Sponsor,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 7,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xD424F1A1F5870000,
        .archipelago_car_id = 0xD424F1A1F5870,
        .archipelago_livery_id = 2042,
        .adjacent_ids = { 0xD424F1A1F64A5000, 0xD424F1A1F6716000, 0xD424F1A1FBA18000, 0xD424F1A2140B8000 },
        .name = "XEUSC2B  - TEMPESTA GT",
        .proper_name = "Rossolini Tempesta GT",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 5,
        .boost = 7,
        .strength = 2,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA7E60B608D960000,
        .archipelago_car_id = 0xA7E60B608D960,
        .archipelago_livery_id = 2043,
        .adjacent_ids = { 0xA7E60B61B96BD000, 0xA7E60B61B992E000, 0xA7E60B60D0B98000, 0xA7E60B60E9238000 },
        .name = "PUSLR01  - OPUS",
        .proper_name = "Carson Opus",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 7,
        .strength = 7,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xD676F93B0B220000,
        .archipelago_car_id = 0xD676F93B0B220,
        .archipelago_livery_id = 2044,
        .adjacent_ids = { 0xD676F93B113C8000, 0xD676F93B17570000, 0xD676F93B4E458000, 0xD676F93B66AF8000 },
        .name = "XUSLRB1  - OPUS XS",
        .proper_name = "Carson Opus XS",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 5,
        .boost = 6,
        .strength = 7,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA7E60F1A40508000,
        .archipelago_car_id = 0xA7E60F1A40508,
        .archipelago_livery_id = 2045,
        .adjacent_ids = { 0xA7E60DA651788000, 0xA7E60DA657930000, 0xA7E60D52A2218000, 0xA7E60D52BA8B8000 },
        .name = "PUSMC02  - ANNIHILATOR",
        .proper_name = "Carson Annihilator",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 5,
        .boost = 7,
        .strength = 6,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xD676FB773F820000,
        .archipelago_car_id = 0xD676FB773F820,
        .archipelago_livery_id = 2046,
        .adjacent_ids = { 0xD676FB77459C8000, 0xD676FB774BB70000, 0xD676FB7782A58000, 0xD676FB779B0F8000 },
        .name = "XUSM2B1  - ANNIHILATOR PHOENIX",
        .proper_name = "Carson Annihilator Phoenix",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 8,
        .strength = 6,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA59403CFE2858000,
        .archipelago_car_id = 0xA59403CFE2858,
        .archipelago_livery_id = 2047,
        .adjacent_ids = { 0xA59402A832B88000, 0xA59402A838D30000, 0xA59403CFE7407000, 0xA59403CFE56BB000 },
        .name = "PEUSC04  - X12",
        .proper_name = "Jansen X12",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 5,
        .boost = 7,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA59402A920C20000,
        .archipelago_car_id = 0xA59402A920C20,
        .archipelago_livery_id = 2048,
        .adjacent_ids = { 0xA59402A926DC8000, 0xA59402A92CF70000, 0xA59402A963E58000, 0xA59402A97C4F8000 },
        .name = "PEUS4B1  - XS12",
        .proper_name = "Jansen XS12",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 4,
        .boost = 8,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0x5950503D2DDCF000,
        .archipelago_car_id = 0x5950503D2DDCF,
        .archipelago_livery_id = 2049,
        .adjacent_ids = {},
        .name = "CARBSC04  - CARBON X12",
        .proper_name = "Jansen Carbon X12",
        .unlock_type = UnlockType::Trophy,
        .category = VehicleCategory::ParadiseCars,
        .speed = 7,
        .boost = 9,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA4FCC2D988700000,
        .archipelago_car_id = 0xA4FCC2D988700,
        .archipelago_livery_id = 2050,
        .adjacent_ids = { 0xA4FCC30861D88000, 0xA4FCC30867F30000, 0xA4FCC2E405FC0000, 0xA4FCC2E7D68C0000 },
        .name = "PASC01   - TOUGE SPORT",
        .proper_name = "Kitano Touge Sport",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 5,
        .boost = 7,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xD38DB0D94FE20000,
        .archipelago_car_id = 0xD38DB0D94FE20,
        .archipelago_livery_id = 2051,
        .adjacent_ids = { 0xD38DB0D955FC8000, 0xD38DB0D95C170000, 0xD38DB0D993058000, 0xD38DB0D9AB6F8000 },
        .name = "XASC1B1  - TOUGE CRITERION",
        .proper_name = "Kitano Touge Criterion",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 5,
        .boost = 8,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA7E6222D0A360000,
        .archipelago_car_id = 0xA7E6222D0A360,
        .archipelago_livery_id = 2052,
        .adjacent_ids = { 0xA7E6223699B88000, 0xA7E622369FD30000, 0xA7E6222D4D598000, 0xA7E6222D65C38000 },
        .name = "PUSPK01  - TAKEDOWN 4X4",
        .proper_name = "Hunter Takedown 4x4",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 7,
        .strength = 10,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xD677100787C20000,
        .archipelago_car_id = 0xD677100787C20,
        .archipelago_livery_id = 2053,
        .adjacent_ids = { 0xD67710078DDC8000, 0xD677100793F70000, 0xD6771007CAE58000, 0xD6771007E34F8000 },
        .name = "XUSPKB1  - TAKEDOWN DIRT RACER",
        .proper_name = "Hunter Takedown Dirt Racer",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 5,
        .boost = 7,
        .strength = 10,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA7E62CE79F360000,
        .archipelago_car_id = 0xA7E62CE79F360,
        .archipelago_livery_id = 2054,
        .adjacent_ids = { 0xA7E62CF12EB88000, 0xA7E62CF134D30000, 0xA7E62CE7E2598000, 0xA7E62CE7FAC38000 },
        .name = "PUSRC01  - 500 GT",
        .proper_name = "Carson 500 GT",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 9,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xD6771AC21CC20000,
        .archipelago_car_id = 0xD6771AC21CC20,
        .archipelago_livery_id = 2055,
        .adjacent_ids = { 0xD6771AC222DC8000, 0xD6771AC228F70000, 0xD6771AC25FE58000, 0xD6771AC2784F8000 },
        .name = "XUSRCB1  - RACING 500 GT",
        .proper_name = "Carson Racing 500 GT",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 5,
        .boost = 10,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA7E62E8B3D160000,
        .archipelago_car_id = 0xA7E62E8B3D160,
        .archipelago_livery_id = 2056,
        .adjacent_ids = { 0xA7E62E94CC988000, 0xA7E62E94D2B30000, 0xA566038412870000, 0xA56603809D848000, 0xA5660380A39F0000, 0xA7E62E8B80398000, 0xA7E62E8B98A38000 },
        .name = "PUSRN01  - RACING OVAL CHAMP",
        .proper_name = "Hunter Racing Oval Champ",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 8,
        .strength = 5,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xD6771C65BAA20000,
        .archipelago_car_id = 0xD6771C65BAA20,
        .archipelago_livery_id = 2057,
        .adjacent_ids = { 0xD6771C65C0BC8000, 0xD6771C65C6D70000, 0xD6771C65FDC58000, 0xD6771C66162F8000 },
        .name = "XUSRN B1  - BRT OVAL CHAMP",
        .proper_name = "Hunter BRT Oval Champ",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 9,
        .strength = 5,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA7E60F1A4C858000,
        .archipelago_car_id = 0xA7E60F1A4C858,
        .archipelago_livery_id = 2058,
        .adjacent_ids = { 0xA7E60DF29CB88000, 0xA7E60DF2A2D30000, 0xA5235AA8AE1CF000, 0xA5235AA8B9408000, 0xA5235AA8BF5B0000, 0xA7E60D548A698000, 0xA7E60D54A2D38000 },
        .name = "PUSMC04  - GT CONCEPT",
        .proper_name = "Carson GT Concept",
        .unlock_type = UnlockType::Sponsor,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 8,
        .strength = 6,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xD676FBC38AC20000,
        .archipelago_car_id = 0xD676FBC38AC20,
        .archipelago_livery_id = 2059,
        .adjacent_ids = { 0xD676FBC390DC8000, 0xD676FBC396F70000, 0xD676FBC3CDE58000, 0xD676FBC3E64F8000 },
        .name = "XUSM4B1  - GT FLAME",
        .proper_name = "Carson GT Flame",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 5,
        .boost = 9,
        .strength = 6,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0x59504F584C1CF000,
        .archipelago_car_id = 0x59504F584C1CF,
        .archipelago_livery_id = 2060,
        .adjacent_ids = {},
        .name = "CARBMC04  - CARBON GT CONCEPT",
        .proper_name = "Carson Carbon GT Concept",
        .unlock_type = UnlockType::Trophy,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 9,
        .strength = 7,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA7E5D3964E17C000,
        .archipelago_car_id = 0xA7E5D3964E17C,
        .archipelago_livery_id = 2061,
        .adjacent_ids = { 0xA7E5D3968B47D000, 0xA7E5D3968B6EE000, 0xA7E5D3964FC57000, 0xA7E5D3965061B000 },
        .name = "PUSCCO01  - CITIZEN",
        .proper_name = "Hunter Citizen",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 8,
        .strength = 8,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xD676C166913B4000,
        .archipelago_car_id = 0xD676C166913B4,
        .archipelago_livery_id = 2062,
        .adjacent_ids = { 0xD676C16691625000, 0xD676C16691896000, 0xD676C16692E8F000, 0xD676C16693853000 },
        .name = "XUSCCOB1  - CIVILIAN",
        .proper_name = "Hunter Civilian",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 5,
        .boost = 9,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA594001623960000,
        .archipelago_car_id = 0xA594001623960,
        .archipelago_livery_id = 2063,
        .adjacent_ids = { 0xA594001FB3188000, 0xA594001FB9330000, 0xA594001666B98000, 0xA59400167F238000 },
        .name = "PEURR01  - 25 V16 REVENGE",
        .proper_name = "Watson 25 V16 Revenge",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 9,
        .strength = 4,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xD424EDF0A1220000,
        .archipelago_car_id = 0xD424EDF0A1220,
        .archipelago_livery_id = 2064,
        .adjacent_ids = { 0xD424EDF0A73C8000, 0xD424EDF0AD570000, 0xD424EDF0E4458000, 0xD424EDF0FCAF8000 },
        .name = "XEURRB1  - REVENGE RACER",
        .proper_name = "Watson Revenge Racer",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 10,
        .strength = 4,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA5940206E8700000,
        .archipelago_car_id = 0xA5940206E8700,
        .archipelago_livery_id = 2065,
        .adjacent_ids = { 0xA594038554D40000, 0xA594038648F80000, 0xA594021165FC0000, 0xA5940215368C0000 },
        .name = "PEUS01   - HAWKER",
        .proper_name = "Montgomery Hawker",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 8,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xD424F17A86500000,
        .archipelago_car_id = 0xD424F17A86500,
        .archipelago_livery_id = 2066,
        .adjacent_ids = { 0xD424F17B7A740000, 0xD424F17C6E980000, 0xD424F18503DC0000, 0xD424F188D46C0000 },
        .name = "XEUSB1   - HAWKER SOLO",
        .proper_name = "Montgomery Hawker Solo",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 9,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0x5950502A6E0EF000,
        .archipelago_car_id = 0x5950502A6E0EF,
        .archipelago_livery_id = 2067,
        .adjacent_ids = {},
        .name = "CARBRWDS  - CARBON HAWKER",
        .proper_name = "Montgomery Carbon Hawker",
        .unlock_type = UnlockType::Trophy,
        .category = VehicleCategory::ParadiseCars,
        .speed = 7,
        .boost = 10,
        .strength = 6,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA593FE7285B60000,
        .archipelago_car_id = 0xA593FE7285B60,
        .archipelago_livery_id = 2068,
        .adjacent_ids = { 0xA593BEDC477C8000, 0xA593BEDC4D970000, 0xA593FE72C8D98000, 0xA593FE72E1438000 },
        .name = "PEURG01  - UBERSCHALL 8",
        .proper_name = "Krieger Uberschall 8",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 7,
        .boost = 9,
        .strength = 2,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xD424EC43B9C93000,
        .archipelago_car_id = 0xD424EC43B9C93,
        .archipelago_livery_id = 2069,
        .adjacent_ids = { 0xD424ACA2F7E93000, 0xD424EC43B92CF000 },
        .name = "XEURG1BG  - UBERSCHALL CLEAR-VIEW",
        .proper_name = "Krieger Uberschall Clear-View",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 10,
        .strength = 2,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0x59504E259C4D8000,
        .archipelago_car_id = 0x59504E259C4D8,
        .archipelago_livery_id = 2070,
        .adjacent_ids = {},
        .name = "CARBEAGT  - CARBON UBERSCHALL 8",
        .proper_name = "Krieger Carbon Uberschall 8",
        .unlock_type = UnlockType::Trophy,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 7,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA7E5D4D6C2D60000,
        .archipelago_car_id = 0xA7E5D4D6C2D60,
        .archipelago_livery_id = 2071,
        .adjacent_ids = { 0xA7E5D4D7EEABD000, 0xA7E5D4D7EED2E000, 0xA7E5D4D705F98000, 0xA7E5D4D71E638000 },
        .name = "PUSCL01  - THUNDER CUSTOM",
        .proper_name = "Carson Thunder Custom",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 8,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xD676C2A7F49F4000,
        .archipelago_car_id = 0xD676C2A7F49F4,
        .archipelago_livery_id = 2072,
        .adjacent_ids = { 0xD676C2A7F6E93000, 0xD676C2A7F64CF000 },
        .name = "XUSCL1B1  - THUNDER SHADOW",
        .proper_name = "Carson Thunder Shadow",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 10,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA7E5CE484B560000,
        .archipelago_car_id = 0xA7E5CE484B560,
        .archipelago_livery_id = 2073,
        .adjacent_ids = { 0xA7E5CE51DAD88000, 0xA7E5CE51E0F30000, 0xA7E5CE488E798000, 0xA7E5CE48A6E38000 },
        .name = "PUSBH01  - HOT ROD COUPE",
        .proper_name = "Carson Hot Rod Coupe",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 5,
        .boost = 10,
        .strength = 5,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xD676BC22C8E20000,
        .archipelago_car_id = 0xD676BC22C8E20,
        .archipelago_livery_id = 2074,
        .adjacent_ids = { 0xD676BC22CEFC8000, 0xD676BC22D5170000, 0xD676BC230C058000, 0xD676BC23246F8000 },
        .name = "XUSBHB1  - TRIBAL SPECIAL",
        .proper_name = "Carson Tribal Special",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 9,
        .strength = 5,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA7E62DCC80F60000,
        .archipelago_car_id = 0xA7E62DCC80F60,
        .archipelago_livery_id = 2075,
        .adjacent_ids = { 0xA7E62DD610788000, 0xA7E62DD616930000, 0xA7E62DCCC4198000, 0xA7E62DCCDC838000 },
        .name = "PUSRI01  - RACING WTR",
        .proper_name = "Krieger Racing WTR",
        .unlock_type = UnlockType::ShutdownRival,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 9,
        .strength = 1,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA7E5D5809C480000,
        .archipelago_car_id = 0xA7E5D5809C480,
        .archipelago_livery_id = 2076,
        .adjacent_ids = { 0xA7E5D58090130000, 0xA7E5D580962D8000, 0xA7E5D580DF6B8000, 0xA7E5D580C7018000 },
        .name = "PUSCPI5  - PCPD SPECIAL",
        .proper_name = "Krieger PCPD Special",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 10,
        .strength = 2,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA7E60F0F736C0000,
        .archipelago_car_id = 0xA7E60F0F736C0,
        .archipelago_livery_id = 2077,
        .adjacent_ids = { 0xA7E60F0F76523000, 0xA7E60F0ECEA08000, 0xA7E60F0ED186B000 },
        .name = "PUSMBST  - FV1100",
        .proper_name = "Nakamura FV1100",
        .unlock_type = UnlockType::Unlock,
        .category = VehicleCategory::ParadiseBikes,
        .speed = 3,
        .boost = 0,
        .strength = 3,
        .boost_type = BoostType::Disabled
    },
    {
        .id = 0xA7E60F0F6DEDC000,
        .archipelago_car_id = 0xA7E60F0F6DEDC,
        .archipelago_livery_id = 2078,
        .adjacent_ids = { 0xA7E60F0F7037B000, 0xA7E60F0F6E14D000, 0xA7E60F0F24D6D000 },
        .name = "PUSMBSS1  - FV1100-TI",
        .proper_name = "Nakamura FV1100-TI",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseBikes,
        .speed = 5,
        .boost = 0,
        .strength = 2,
        .boost_type = BoostType::Disabled
    },
    {
        .id = 0xA7E60EF414960000,
        .archipelago_car_id = 0xA7E60EF414960,
        .archipelago_livery_id = 2079,
        .adjacent_ids = { 0xA7E60EF564478000, 0xA7E60EF41AB08000, 0xA7E60EF6586B8000 },
        .name = "PUSMB01  - FIREHAWK V4",
        .proper_name = "Nakamura Firehawk V4",
        .unlock_type = UnlockType::Unlock,
        .category = VehicleCategory::ParadiseBikes,
        .speed = 6,
        .boost = 0,
        .strength = 1,
        .boost_type = BoostType::Disabled
    },
    {
        .id = 0xA7E60F03E9EE4000,
        .archipelago_car_id = 0xA7E60F03E9EE4,
        .archipelago_livery_id = 2080,
        .adjacent_ids = { 0xA7E60F0357995000, 0xA7E60F0357C06000, 0xA7E60F0357E77000, 0xA7E60F03580E8000, 0xA7E60F0358359000, 0xA7E60F03585CA000, 0xA7E60F035883B000 },
        .name = "PUSMBGP1  - FIREHAWK GP COMPETITION",
        .proper_name = "Nakamura Firehawk GP Competition",
        .unlock_type = UnlockType::Gift,
        .category = VehicleCategory::ParadiseBikes,
        .speed = 8,
        .boost = 0,
        .strength = 0,
        .boost_type = BoostType::Disabled
    },
    {
        .id = 0xA5235611C067C000,
        .archipelago_car_id = 0xA5235611C067C,
        .archipelago_livery_id = 2081,
        .adjacent_ids = { 0xA56CF94E4D3F0000, 0xA56CF94ECD6B8000 },
        .name = "PBTSVK01  - OLYMPUS",
        .proper_name = "Hunter Olympus",
        // .unlock_type = UnlockType::OnlineOnly,
        .unlock_type = UnlockType::Unlock,
        // .category = VehicleCategory::OnlineCars,
        .category = VehicleCategory::ParadiseCars,
        .speed = 6,
        .boost = 0,
        .strength = 10,
        .boost_type = BoostType::Disabled
    },
    {
        .id = 0xA56C12301BD60000,
        .archipelago_car_id = 0xA56C12301BD60,
        .archipelago_livery_id = 2082,
        .adjacent_ids = { 0xA56C124428E48000, 0xA56C12442EFF0000 },
        .name = "PDRWT01  - RAI-JIN TURBO",
        .proper_name = "Nakamura Rai-Jin Turbo",
        // .unlock_type = UnlockType::OnlineOnly,
        .unlock_type = UnlockType::Unlock,
        // .category = VehicleCategory::OnlineCars,
        .category = VehicleCategory::ParadiseCars,
        .speed = 10,
        .boost = 0,
        .strength = 4,
        .boost_type = BoostType::Disabled
    },
    {
        .id = 0xA78D955662360000,
        .archipelago_car_id = 0xA78D955662360,
        .archipelago_livery_id = 2083,
        .adjacent_ids = {},
        .name = "PSDMC01  - TOY CAVALRY",
        .proper_name = "Hunter Toy Cavalry",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::ToyVehicles,
        .speed = 1,
        .boost = 1,
        .strength = 5,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA78D955674858000,
        .archipelago_car_id = 0xA78D955674858,
        .archipelago_livery_id = 2084,
        .adjacent_ids = {},
        .name = "PSDMC04  - TOY GT CONCEPT",
        .proper_name = "Carson Toy GT Concept",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::ToyVehicles,
        .speed = 2,
        .boost = 4,
        .strength = 6,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA78DB408A8F60000,
        .archipelago_car_id = 0xA78DB408A8F60,
        .archipelago_livery_id = 2085,
        .adjacent_ids = {},
        .name = "PSDRI01  - TOY WTR",
        .proper_name = "Krieger Toy WTR",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::ToyVehicles,
        .speed = 3,
        .boost = 5,
        .strength = 1,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA78DB919A8360000,
        .archipelago_car_id = 0xA78DB919A8360,
        .archipelago_livery_id = 2086,
        .adjacent_ids = {},
        .name = "PSDSC01  - TOY P12",
        .proper_name = "Jansen Toy P12",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::ToyVehicles,
        .speed = 2,
        .boost = 3,
        .strength = 3,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA78D53C5B7360000,
        .archipelago_car_id = 0xA78D53C5B7360,
        .archipelago_livery_id = 2087,
        .adjacent_ids = {},
        .name = "PSDBC01  - TOY MANHATTAN",
        .proper_name = "Hunter Toy Manhattan",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::ToyVehicles,
        .speed = 1,
        .boost = 3,
        .strength = 7,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA78DA86932360000,
        .archipelago_car_id = 0xA78DA86932360,
        .archipelago_livery_id = 2088,
        .adjacent_ids = {},
        .name = "PSDPK01  - TOY TAKEDOWN 4X4",
        .proper_name = "Hunter Toy Takedown 4x4",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::ToyVehicles,
        .speed = 2,
        .boost = 4,
        .strength = 10,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA78D59D27617C000,
        .archipelago_car_id = 0xA78D59D27617C,
        .archipelago_livery_id = 2089,
        .adjacent_ids = {},
        .name = "PSDCCO01  - TOY CITIZEN",
        .proper_name = "Hunter Toy Citizen",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::ToyVehicles,
        .speed = 2,
        .boost = 4,
        .strength = 8,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA78D5C9063160000,
        .archipelago_car_id = 0xA78D5C9063160,
        .archipelago_livery_id = 2090,
        .adjacent_ids = {},
        .name = "PSDCV01  - TOY INFERNO",
        .proper_name = "Carson Toy Inferno",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::ToyVehicles,
        .speed = 2,
        .boost = 3,
        .strength = 10,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA78D738010800000,
        .archipelago_car_id = 0xA78D738010800,
        .archipelago_livery_id = 2091,
        .adjacent_ids = {},
        .name = "PSDGP    - TOY FIREHAWK GP",
        .proper_name = "Nakamura Toy Firehawk GP",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::ToyVehicles,
        .speed = 5,
        .boost = 0,
        .strength = 3,
        .boost_type = BoostType::Disabled
    },
    {
        .id = 0xA56601CB30510000,
        .archipelago_car_id = 0xA56601CB30510,
        .archipelago_livery_id = 2092,
        .adjacent_ids = {},
        .name = "PDLCB2F  - P12 88 SPECIAL",
        .proper_name = "Jansen P12 88 Special",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::LegendaryCars,
        .speed = 6,
        .boost = 9,
        .strength = 3,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA566029213D80000,
        .archipelago_car_id = 0xA566029213D80,
        .archipelago_livery_id = 2093,
        .adjacent_ids = {},
        .name = "PDLCGB   - MANHATTAN SPIRIT",
        .proper_name = "Hunter Manhattan Spirit",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::LegendaryCars,
        .speed = 5,
        .boost = 7,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA56603AC7063C000,
        .archipelago_car_id = 0xA56603AC7063C,
        .archipelago_livery_id = 2094,
        .adjacent_ids = {},
        .name = "PDLCNR01 - GT NIGHTHAWK",
        .proper_name = "Carson GT Nighthawk",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::LegendaryCars,
        .speed = 6,
        .boost = 9,
        .strength = 10,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA566029B9D400000,
        .archipelago_car_id = 0xA566029B9D400,
        .archipelago_livery_id = 2095,
        .adjacent_ids = {},
        .name = "PDLCGL   - CAVALRY BOOTLEGGER",
        .proper_name = "Hunter Cavalry Bootlegger",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::LegendaryCars,
        .speed = 3,
        .boost = 5,
        .strength = 5,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA56CA6E6C9960000,
        .archipelago_car_id = 0xA56CA6E6C9960,
        .archipelago_livery_id = 2096,
        .adjacent_ids = {},
        .name = "PDSHR01  - EXTREME HOT ROD",
        .proper_name = "Carson Extreme Hot Rod",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::BoostSpecialCars,
        .speed = 7,
        .boost = 10,
        .strength = 8,
        .boost_type = BoostType::Infinite
    },
    {
        .id = 0xA5231316B5760000,
        .archipelago_car_id = 0xA5231316B5760,
        .archipelago_livery_id = 2097,
        .adjacent_ids = { 0xA5231316F27F0000, 0xA5231316F8998000 },
        .name = "PBTHM01  - HAWKER MECH",
        .proper_name = "Montgomery Hawker Mech",
        .unlock_type = UnlockType::DLC,
        .category = VehicleCategory::BoostSpecialCars,
        .speed = 6,
        .boost = 8,
        .strength = 4,
        .boost_type = BoostType::Multi
    },
    {
        .id = 0xA5389B073A360000,
        .archipelago_car_id = 0xA5389B073A360,
        .archipelago_livery_id = 2098,
        .adjacent_ids = {},
        .name = "PCCMC01  - PCPD CAVALRY",
        .proper_name = "Hunter PCPD Cavalry",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 1,
        .boost = 1,
        .strength = 5,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA53860DF71048000,
        .archipelago_car_id = 0xA53860DF71048,
        .archipelago_livery_id = 2099,
        .adjacent_ids = {},
        .name = "PCCCLT2  - PCPD MESQUITE",
        .proper_name = "Hunter PCPD Mesquite",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 1,
        .boost = 2,
        .strength = 6,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA5385BE45567C000,
        .archipelago_car_id = 0xA5385BE45567C,
        .archipelago_livery_id = 2100,
        .adjacent_ids = {},
        .name = "PCCBSC01  - PCPD SI-7",
        .proper_name = "Nakamura PCPD SI-7",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 1,
        .boost = 2,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA538613633B60000,
        .archipelago_car_id = 0xA538613633B60,
        .archipelago_livery_id = 2101,
        .adjacent_ids = {},
        .name = "PCCCO01  - PCPD VEGAS",
        .proper_name = "Hunter PCPD Vegas",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 1,
        .boost = 2,
        .strength = 6,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA538C19F4B160000,
        .archipelago_car_id = 0xA538C19F4B160,
        .archipelago_livery_id = 2102,
        .adjacent_ids = {},
        .name = "PCCSV01  - PCPD PIONEER",
        .proper_name = "Krieger PCPD Pioneer",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 1,
        .boost = 2,
        .strength = 8,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA5385BD8E9360000,
        .archipelago_car_id = 0xA5385BD8E9360,
        .archipelago_livery_id = 2103,
        .adjacent_ids = {},
        .name = "PCCBS01  - PCPD IKUSA GT",
        .proper_name = "Nakamura PCPD Ikusa GT",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 2,
        .boost = 3,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA5385981FB67C000,
        .archipelago_car_id = 0xA5385981FB67C,
        .archipelago_livery_id = 2104,
        .adjacent_ids = {},
        .name = "PCCBCC01  - PCPD HYDROS CUSTOM",
        .proper_name = "Kitano PCPD Hydros Custom",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 2,
        .boost = 3,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA53861F4EFD60000,
        .archipelago_car_id = 0xA53861F4EFD60,
        .archipelago_livery_id = 2105,
        .adjacent_ids = {},
        .name = "PCCCT01  - PCPD RELIABLE CUSTOM",
        .proper_name = "Hunter PCPD Reliable Custom",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 2,
        .boost = 3,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA5385BB2C3960000,
        .archipelago_car_id = 0xA5385BB2C3960,
        .archipelago_livery_id = 2106,
        .adjacent_ids = {},
        .name = "PCCBR01  - PCPD R-TURBO ROADSTER",
        .proper_name = "Watson PCPD R-Turbo Roadster",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 2,
        .boost = 3,
        .strength = 3,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA538968ED1760000,
        .archipelago_car_id = 0xA538968ED1760,
        .archipelago_livery_id = 2107,
        .adjacent_ids = {},
        .name = "PCCLM01  - PCPD LM CLASSIC",
        .proper_name = "Rossolini PCPD LM Classic",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 2,
        .boost = 4,
        .strength = 2,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA53859768F360000,
        .archipelago_car_id = 0xA53859768F360,
        .archipelago_livery_id = 2108,
        .adjacent_ids = {},
        .name = "PCCBC01  - PCPD MANHATTAN",
        .proper_name = "Hunter PCPD Manhattan",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 3,
        .boost = 4,
        .strength = 7,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA5389DB8A3780000,
        .archipelago_car_id = 0xA5389DB8A3780,
        .archipelago_livery_id = 2109,
        .adjacent_ids = {},
        .name = "PCCMU3   - PCPD FASTBACK",
        .proper_name = "Carson PCPD Fastback",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 3,
        .boost = 4,
        .strength = 5,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA53876F7A8F60000,
        .archipelago_car_id = 0xA53876F7A8F60,
        .archipelago_livery_id = 2110,
        .adjacent_ids = {},
        .name = "PCCGA01  - PCPD GRAND MARAIS",
        .proper_name = "Carson PCPD Grand Marais",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 3,
        .boost = 5,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA538C106B4960000,
        .archipelago_car_id = 0xA538C106B4960,
        .archipelago_livery_id = 2111,
        .adjacent_ids = {},
        .name = "PCCSR01  - PCPD HYPERION",
        .proper_name = "Montgomery PCPD Hyperion",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 3,
        .boost = 4,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA538BECA8C6B0000,
        .archipelago_car_id = 0xA538BECA8C6B0,
        .archipelago_livery_id = 2112,
        .adjacent_ids = {},
        .name = "PCCSC03  - PCPD 616 SPORT",
        .proper_name = "Krieger PCPD 616 Sport",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 3,
        .boost = 5,
        .strength = 5,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA5389B5385760000,
        .archipelago_car_id = 0xA5389B5385760,
        .archipelago_livery_id = 2113,
        .adjacent_ids = {},
        .name = "PCCME01  - PCPD SPUR",
        .proper_name = "Hunter PCPD Spur",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 4,
        .boost = 5,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA538B8D4AB6B0000,
        .archipelago_car_id = 0xA538B8D4AB6B0,
        .archipelago_livery_id = 2114,
        .adjacent_ids = {},
        .name = "PCCRC03  - PCPD GT 2400",
        .proper_name = "Montgomery PCPD GT 2400",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 4,
        .boost = 6,
        .strength = 2,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA538BECA80360000,
        .archipelago_car_id = 0xA538BECA80360,
        .archipelago_livery_id = 2115,
        .adjacent_ids = {},
        .name = "PCCSC01  - PCPD P12",
        .proper_name = "Jansen PCPD P12",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 4,
        .boost = 6,
        .strength = 3,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA53862413B160000,
        .archipelago_car_id = 0xA53862413B160,
        .archipelago_livery_id = 2116,
        .adjacent_ids = {},
        .name = "PCCCV01  - PCPD INFERNO VAN",
        .proper_name = "Carson PCPD Inferno Van",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 4,
        .boost = 6,
        .strength = 10,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA538BECA86508000,
        .archipelago_car_id = 0xA538BECA86508,
        .archipelago_livery_id = 2117,
        .adjacent_ids = {},
        .name = "PCCSC02  - PCPD TEMPESTA",
        .proper_name = "Rossolini PCPD Tempesta",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 4,
        .boost = 7,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA538974D8D960000,
        .archipelago_car_id = 0xA538974D8D960,
        .archipelago_livery_id = 2118,
        .adjacent_ids = {},
        .name = "PCCLR01  - PCPD OPUS",
        .proper_name = "Carson PCPD Opus",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 4,
        .boost = 7,
        .strength = 7,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA5389B0740508000,
        .archipelago_car_id = 0xA5389B0740508,
        .archipelago_livery_id = 2119,
        .adjacent_ids = {},
        .name = "PCCMC02  - PCPD ANNIHILATOR",
        .proper_name = "Carson PCPD Annihilator",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 5,
        .boost = 7,
        .strength = 6,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA538BECA92858000,
        .archipelago_car_id = 0xA538BECA92858,
        .archipelago_livery_id = 2120,
        .adjacent_ids = {},
        .name = "PCCSC04  - PCPD X12",
        .proper_name = "Jansen PCPD X12",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 5,
        .boost = 7,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA5385DA388700000,
        .archipelago_car_id = 0xA5385DA388700,
        .archipelago_livery_id = 2121,
        .adjacent_ids = {},
        .name = "PCCC01   - PCPD TOUGE SPORT",
        .proper_name = "Kitano PCPD Touge Sport",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 5,
        .boost = 7,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA538AE1A0A360000,
        .archipelago_car_id = 0xA538AE1A0A360,
        .archipelago_livery_id = 2122,
        .adjacent_ids = {},
        .name = "PCCPK01  - PCPD TAKEDOWN 4X4",
        .proper_name = "Hunter PCPD Takedown 4x4",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 6,
        .boost = 7,
        .strength = 10,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA538B8D49F360000,
        .archipelago_car_id = 0xA538B8D49F360,
        .archipelago_livery_id = 2123,
        .adjacent_ids = {},
        .name = "PCCRC01  - PCPD 500 GT",
        .proper_name = "Carson PCPD 500 GT",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 6,
        .boost = 9,
        .strength = 3,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA538BA783D160000,
        .archipelago_car_id = 0xA538BA783D160,
        .archipelago_livery_id = 2124,
        .adjacent_ids = {},
        .name = "PCCRN01  - PCPD RACING OVAL CHAMP",
        .proper_name = "Hunter PCPD Racing Oval Champ",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 6,
        .boost = 8,
        .strength = 5,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA5389B074C858000,
        .archipelago_car_id = 0xA5389B074C858,
        .archipelago_livery_id = 2125,
        .adjacent_ids = {},
        .name = "PCCMC04  - PCPD GT CONCEPT",
        .proper_name = "Carson PCPD GT Concept",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 6,
        .boost = 8,
        .strength = 6,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA538BD0198700000,
        .archipelago_car_id = 0xA538BD0198700,
        .archipelago_livery_id = 2126,
        .adjacent_ids = {},
        .name = "PCCS01   - PCPD HAWKER",
        .proper_name = "Montgomery PCPD Hawker",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 6,
        .boost = 8,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA538B96D35B60000,
        .archipelago_car_id = 0xA538B96D35B60,
        .archipelago_livery_id = 2127,
        .adjacent_ids = {},
        .name = "PCCRG01  - PCPD UBERSCHALL 8",
        .proper_name = "Krieger PCPD Uberschall 8",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 7,
        .boost = 9,
        .strength = 2,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA5385F6C70360000,
        .archipelago_car_id = 0xA5385F6C70360,
        .archipelago_livery_id = 2128,
        .adjacent_ids = {},
        .name = "PCCCC01  - PCPD THUNDER CUSTOM",
        .proper_name = "Carson PCPD Thunder Custom",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 6,
        .boost = 8,
        .strength = 7,
        .boost_type = BoostType::Crash
    },
    {
        .id = 0xA5385A354B560000,
        .archipelago_car_id = 0xA5385A354B560,
        .archipelago_livery_id = 2129,
        .adjacent_ids = {},
        .name = "PCCBH01  - PCPD HOT ROD COUPE",
        .proper_name = "Carson PCPD Hot Rod Coupe",
        .unlock_type = UnlockType::Cop,
        .category = VehicleCategory::CopCars,
        .speed = 5,
        .boost = 10,
        .strength = 5,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA55EBC7BF8700000,
        .archipelago_car_id = 0xA55EBC7BF8700,
        .archipelago_livery_id = 2130,
        .adjacent_ids = { 0xA55EBC848DB40000, 0xA55EBC8675FC0000 },
        .name = "PDDK01   - DUST STORM",
        .proper_name = "Carson Dust Storm",
        .unlock_type = UnlockType::IslandGift,
        .category = VehicleCategory::BigSurfIslandCars,
        .speed = 3,
        .boost = 4,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA55EBC7CD8E4F000,
        .archipelago_car_id = 0xA55EBC7CD8E4F,
        .archipelago_livery_id = 2131,
        .adjacent_ids = {},
        .name = "PDDK01XS  - DUST STORM SUPERTURBO",
        .proper_name = "Carson Dust Storm Superturbo",
        .unlock_type = UnlockType::IslandGift,
        .category = VehicleCategory::BigSurfIslandCars,
        .speed = 4,
        .boost = 6,
        .strength = 4,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA5231F81B5EC8000,
        .archipelago_car_id = 0xA5231F81B5EC8,
        .archipelago_livery_id = 2132,
        .adjacent_ids = {},
        .name = "PBTJPDI  - P12 DIAMOND",
        .proper_name = "Jansen P12 Diamond",
        .unlock_type = UnlockType::IslandGift,
        .category = VehicleCategory::BigSurfIslandCars,
        .speed = 6,
        .boost = 7,
        .strength = 3,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA5235611C08ED000,
        .archipelago_car_id = 0xA5235611C08ED,
        .archipelago_livery_id = 2133,
        .adjacent_ids = {},
        .name = "PBTSVK02  - OLYMPUS GOVERNOR",
        .proper_name = "Hunter Olympus Governor",
        .unlock_type = UnlockType::IslandGift,
        .category = VehicleCategory::BigSurfIslandCars,
        .speed = 6,
        .boost = 7,
        .strength = 10,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA522FF6B4EF60000,
        .archipelago_car_id = 0xA522FF6B4EF60,
        .archipelago_livery_id = 2134,
        .adjacent_ids = { 0xA522FF6C80230000, 0xA522FF6C863D8000 },
        .name = "PBTEA01  - ANNIHILATOR STREET ROD",
        .proper_name = "Carson Annihilator Street Rod",
        .unlock_type = UnlockType::IslandGift,
        .category = VehicleCategory::BigSurfIslandCars,
        .speed = 6,
        .boost = 10,
        .strength = 6,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA78D72EA6ED60000,
        .archipelago_car_id = 0xA78D72EA6ED60,
        .archipelago_livery_id = 2135,
        .adjacent_ids = {},
        .name = "PSDGL01  - TOY BOOTLEGGER",
        .proper_name = "Hunter Toy Bootlegger",
        .unlock_type = UnlockType::IslandUnlock,
        .category = VehicleCategory::BigSurfIslandCars,
        .speed = 2,
        .boost = 4,
        .strength = 5,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA78D4166193C0000,
        .archipelago_car_id = 0xA78D4166193C0,
        .archipelago_livery_id = 2136,
        .adjacent_ids = {},
        .name = "PSD88S   - TOY 88 SPECIAL",
        .proper_name = "Jansen Toy 88 Special",
        .unlock_type = UnlockType::IslandUnlock,
        .category = VehicleCategory::BigSurfIslandCars,
        .speed = 2,
        .boost = 5,
        .strength = 3,
        .boost_type = BoostType::Stunt
    },
    {
        .id = 0xA78D9D8877960000,
        .archipelago_car_id = 0xA78D9D8877960,
        .archipelago_livery_id = 2137,
        .adjacent_ids = {},
        .name = "PSDNR01  - TOY NIGHTHAWK",
        .proper_name = "Carson Toy Nighthawk",
        .unlock_type = UnlockType::IslandUnlock,
        .category = VehicleCategory::BigSurfIslandCars,
        .speed = 4,
        .boost = 6,
        .strength = 10,
        .boost_type = BoostType::Speed
    },
    {
        .id = 0xA78D716A01C00000,
        .archipelago_car_id = 0xA78D716A01C00,
        .adjacent_ids = {},
        .name = "PSDGB    - TOY SPIRIT",
        .proper_name = "Hunter Toy Spirit",
        .unlock_type = UnlockType::IslandUnlock,
        .category = VehicleCategory::BigSurfIslandCars,
        .speed = 2,
        .boost = 4,
        .strength = 7,
        .boost_type = BoostType::Crash
    }
};


inline const VehicleInfo* FindVehicleByID(uint64_t vehicleID)
{
    for (const VehicleInfo& vehicle : vehicleInfos)
    {
        if (vehicle.id == vehicleID)
        {
            return &vehicle;
        }
    }

    return nullptr;
}

inline const VehicleInfo* FindVehicleByLiveryID(uint64_t liveryID)
{
    for (const VehicleInfo& vehicle : vehicleInfos)
    {
        for(uint64_t liveryId : vehicle.adjacent_ids)
            if (liveryId == liveryID)
            {
                return &vehicle;
            }
    }

    return nullptr;
}

inline const VehicleInfo* FindVehicleByArchipelagoCarID(uint64_t archipelago_id)
{
    for (const VehicleInfo& vehicle : vehicleInfos)
    {
        if (vehicle.archipelago_car_id == archipelago_id)
        {
            return &vehicle;
        }
    }

    return nullptr;
}

inline const VehicleInfo* FindVehicleByArchipelagoLiveryID(uint64_t archipelago_id)
{
    for (const VehicleInfo& vehicle : vehicleInfos)
    {
        if (vehicle.archipelago_livery_id == archipelago_id)
        {
            return &vehicle;
        }
    }

    return nullptr;
}

inline std::vector<const VehicleInfo*> FindAllByUnlockType(UnlockType type)
{
    std::vector<const VehicleInfo*> result;
    for (const VehicleInfo& v : vehicleInfos)
    {
        if (v.unlock_type == type)
        {
            result.push_back(&v);
        }
    }
    return result;
}

inline std::vector<const VehicleInfo*> FindAllByCategory(VehicleCategory category)
{
    std::vector<const VehicleInfo*> result;
    for (const VehicleInfo& v : vehicleInfos)
    {
        if (v.category == category)
        {
            result.push_back(&v);
        }
    }
    return result;
}

inline std::vector<const VehicleInfo*> FindAllByBoostType(BoostType type)
{
    std::vector<const VehicleInfo*> result;
    for (const VehicleInfo& v : vehicleInfos)
    {
        if (v.boost_type == type)
        {
            result.push_back(&v);
        }
    }
    return result;
}

