#include "MinHook.h"
#include "bpr/core/logger.hpp"
#include <format>
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

    using Fn = std::uint32_t (__thiscall*)(void* profile, std::uint32_t idLow,
                                           std::int32_t idHigh, std::int32_t type);
    static Fn Original = nullptr;

    static void OnVisited(std::int32_t type, std::uint64_t id)
    {
        Logger::Log(std::format("Drive-thru visited: Type {}, ID {}", type, id));
        App::Instance->Gui().info_window->AddLogMessage(std::format("Drive-thru visited: Type {} ID {}", type, id));
        // TODO: App::Instance->State().SendLocation(<location for (type, id)>);
    }

    static std::uint32_t __fastcall Detour(void* profile, void*, std::uint32_t lo, std::int32_t hi, std::int32_t type)
    {
        const std::uint64_t id = (static_cast<std::uint64_t>(static_cast<std::uint32_t>(hi)) << 32) | lo;

        OnVisited(type, id);
        // if (!IsGranted(type, id)) TODO: slot_data cache drive-thru discoveries
        //     return 0;


        return Original(profile, lo, hi, type);
    }

    bool GrantDiscovery(std::int32_t type, std::uint64_t id)
    {
        auto* game = *reinterpret_cast<std::uint8_t**>(0x013FC8E0);
        if (!game || !Original) return false;
        Original(game + 0x6A7B80, static_cast<std::uint32_t>(id), static_cast<std::int32_t>(id >> 32), type);
        return true;
    }

    MH_STATUS Install()
    {
        return MH_CreateHook(reinterpret_cast<void*>(Address),
                             reinterpret_cast<void*>(&Detour),
                             reinterpret_cast<void**>(&Original));
    }
}
