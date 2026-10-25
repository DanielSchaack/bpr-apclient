#include "MinHook.h"
#include "bpr/core/logger.hpp"
#include <format>
#include "bpr/core/map.hpp"
#include "detours.hpp"
#include "../../app/app.hpp"

namespace DetectDriveThru
{
    constexpr std::uintptr_t Address = 0x00A0A720;

    struct SetInfo { std::int32_t type; std::size_t setOff; std::size_t countOff; std::size_t cap; };
    static constexpr SetInfo kSets[] = {
        {0, 0xABB0, 0xABD8, 5},    // junkyard
        {2, 0xAC70, 0xACE0, 14},   // gas station
        {3, 0xABE0, 0xAC38, 11},   // auto repair
        {4, 0xAC40, 0xAC68, 5},    // paint shop
        {5, 0xACE8, 0xAD40, 11},   // car park
    };

    struct IslandSet { std::int32_t type; std::size_t idOff; std::size_t countOff; };
    static constexpr IslandSet kIslandSets[] = {
        {0, 0x2873C, 0x28740},
        {3, 0x28744, 0x28748},
    };

    using Fn = std::uint32_t (__thiscall*)(void* profile, std::uint32_t idLow,
                                           std::int32_t idHigh, std::int32_t type);
    static Fn Original = nullptr;

    static void OnVisited(std::int32_t type, std::uint64_t id)
    {
        // Logger::Log(std::format("Drive-thru visited: Type {}, ID {}", type, id));
        App::Instance->Gui().info_window->AddLogMessage(std::format("Drive-thru visited: Type {} ID {}", type, id));
        const Map::DriveThruInfo* info = Map::FindDriveThruByID(id);
        if(info)
            App::Instance->State().CacheBreakable(static_cast<uint8_t>(info->area), 3);

        if (!App::Instance->State().GetSlotData().includeDriveThrusJumpPoints && info && info->type == Map::DriveThruType::JUNKYARD)
            App::Instance->State().GetSaveData().obtained_drivethru_jumppoints.insert(id);

        App::Instance->State().GetSaveData().visited_drivethrus.insert(id);
    }

    static std::uint32_t __fastcall Detour(void* profile, void*, std::uint32_t lo, std::int32_t hi, std::int32_t type)
    {
        const std::uint64_t id = (static_cast<std::uint64_t>(static_cast<std::uint32_t>(hi)) << 32) | lo;

        OnVisited(type, id);

        return Original(profile, lo, hi, type);
    }

    static bool IsIslandId(std::uint64_t id)
    {
        return id > 620000 || (id >> 32) != 0;
    }

    static std::uint8_t* LiveProfile()
    {
        auto* game = *reinterpret_cast<std::uint8_t**>(0x013FC8E0);
        if (!game || *reinterpret_cast<std::uint32_t*>(game + 0xB6D464) != 1)
            return nullptr;
        return game + 0x6A7B80;
    }

    bool GrantDiscovery(std::uint64_t id)
    {
        const Map::DriveThruInfo* info = Map::FindDriveThruByID(id);
        auto* profile = LiveProfile();
        if (!info || !profile || !Original)
            return false;

        // the dispatcher does the right thing for both base and island IDs
        Original(profile, static_cast<std::uint32_t>(id), static_cast<std::int32_t>(id >> 32),
                static_cast<std::int32_t>(info->type));
        return true;
    }

    bool RevokeDiscovery(std::uint64_t id)
    {
        const Map::DriveThruInfo* info = Map::FindDriveThruByID(id);
        auto* profile = LiveProfile();
        if (!info || !profile)
            return false;

        const std::int32_t type = static_cast<std::int32_t>(info->type);

        if (IsIslandId(id))
        {
            for (const auto& s : kIslandSets)
            {
                if (s.type != type) continue;
                auto& stored = *reinterpret_cast<std::uint32_t*>(profile + s.idOff);
                auto& count  = *reinterpret_cast<std::int32_t*>(profile + s.countOff);
                if (count <= 0 || stored != static_cast<std::uint32_t>(id))
                    return false;                              // not the shop in this slot
                stored = 0;
                count = 0;
                Logger::Log(std::format("Revoked island drive-thru: Type {}, ID {}", type, id));
                return true;
            }
            return false;                                      // no island set known for this type
        }

        for (const auto& s : kSets)
        {
            if (s.type != type) continue;
            auto* ids   = reinterpret_cast<std::uint64_t*>(profile + s.setOff);
            auto& count = *reinterpret_cast<std::int32_t*>(profile + s.countOff);
            if (count <= 0 || count > static_cast<std::int32_t>(s.cap))
                return false;
            for (std::int32_t i = 0; i < count; ++i)
            {
                if (ids[i] != id) continue;
                std::memmove(&ids[i], &ids[i + 1], (count - 1 - i) * sizeof(std::uint64_t));
                ids[count - 1] = 0;
                --count;
                Logger::Log(std::format("Revoked drive-thru: Type {}, ID {} (count now {})", type, id, count));
                return true;
            }
            return false;
        }
        return false;
    }

    MH_STATUS Install()
    {
        return MH_CreateHook(reinterpret_cast<void*>(Address),
                             reinterpret_cast<void*>(&Detour),
                             reinterpret_cast<void**>(&Original));
    }
}
