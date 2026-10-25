#include "bpr/core/logger.hpp"
#include "bpr/core/map.hpp"
#include "detours.hpp"
#include "MinHook.h"
#include <format>
#include <windows.h>
#include "../../app/app.hpp"

namespace DetectBreakable
{
    using AreaOfFn = std::uint32_t (__thiscall*)(void* self, void* obj);
    static const auto AreaOfObject = reinterpret_cast<AreaOfFn>(0x03242F20);

    extern "C" __declspec(noinline) void __stdcall
    LogBreakable(std::uint32_t type, std::uint32_t id, std::uint32_t area) noexcept
    {
        App::Instance->Gui().info_window->AddLogMessage(std::format("{} {} ID: {}", Map::areaIndex[area], Map::typeIndex[type], id));
        Logger::Log(std::format("{} {} ID: {}", Map::areaIndex[area], Map::typeIndex[type], id));
        App::Instance->State().CacheBreakable(area, type);
    }

    constexpr std::uintptr_t RegisterAddress = 0x03214990;
    constexpr std::uintptr_t CollectAddress  = 0x00A14930;

    static int CountyOf(std::uint32_t area)                  // mirrors AreaOfFn
    {
        if (area > 0x16)  return 6;
        if (area <= 3)    return 0;
        if (area <= 6)    return 1;
        if (area <= 10)   return 2;
        if (area <= 0xD)  return 3;
        if (area <= 0x11) return 4;
        return 5;
    }

    using RegisterFn = std::uint32_t (__thiscall*)(void*, std::int32_t, std::uint32_t, std::int32_t, std::int32_t);
    static RegisterFn OriginalRegister = nullptr;

    // Triggers on the specific Breakable
    static std::uint32_t __fastcall HkRegister(void* profile, void*, std::int32_t type,
                                               std::uint32_t idLo, std::int32_t idHi, std::int32_t area)
    {
        const std::uint64_t id = (static_cast<std::uint64_t>(static_cast<std::uint32_t>(idHi)) << 32) | idLo;

        const int county = CountyOf(area);
        Logger::Log(std::format("{} {} ID: {}", county < 6 ? Map::areaIndex[county] : "?", Map::typeIndex[type], id));
        App::Instance->Gui().info_window->AddLogMessage(std::format("{} {} ID: {}", county < 6 ? Map::areaIndex[county] : "?", Map::typeIndex[type], id));
        App::Instance->State().CacheBreakable(county, type);

        return OriginalRegister(profile, type, idLo, idHi, area);
    }

    // Triggers on any object of a Breakable, controls the flow
    using CollectFn = void (__thiscall*)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    static CollectFn OriginalCollect = nullptr;

    static void __fastcall HkCollect(void* self, void*, std::uint32_t a2, std::uint32_t a3,
                                     std::uint32_t a4, std::uint32_t a5)
    {
        auto* s = static_cast<std::uint8_t*>(self);
        const bool isJump = static_cast<std::uint8_t>(a3) != 0;
        const bool flagB  = static_cast<std::uint8_t>(a5) != 0;

        // same object/ID selection the game does at the top of the function
        auto* obj = *reinterpret_cast<std::uint8_t**>(s + (isJump ? 0x3B8 : 0x3A8));
        std::int32_t id = 0;
        if (obj)
        {
            id = *reinterpret_cast<std::int32_t*>(obj + 0x2C);
            if (id == 0) id = *reinterpret_cast<std::int32_t*>(obj + 0x24);
        }
        const std::int32_t type = isJump ? 0 : *reinterpret_cast<std::int32_t*>(s + 0x3AC);

        auto* manager = *reinterpret_cast<std::uint8_t**>(s + 0x390);
        const std::uint32_t unit = *reinterpret_cast<std::uint16_t*>(s + 0x3B0);
        const std::uint32_t prop = *reinterpret_cast<std::uint16_t*>(s + 0x3B2);
        const std::uint32_t bit  = unit * 600u + prop;
        const bool track = manager && !isJump && bit < 300000;
        auto* qwords = track ? reinterpret_cast<std::uint64_t*>(manager + 0xEF78) : nullptr;   // profile + 0xEE08

        // Same condition as ApState::CacheBreakable: only the owned/unlocked case may collect.
        const int area = CountyOf(AreaOfObject(self, obj));
        if ((!App::Instance->State().GetSlotData().cacheBreakables && App::Instance->State().GetSlotData().lockBreakables && !App::Instance->State().GetSaveData().breakable_owned[area][type]))
        {
            if(type != 0)
            {
                qwords[bit >> 6] &= ~(1ULL << (bit & 63));              // undo only a bit this call set
                Logger::Log("  -> bit cleared (collection blocked)");
            }
            return;                                          // not owned yet: no bit, no popup, no registration
        }

        const bool before = track && ((qwords[bit >> 6] >> (bit & 63)) & 1);
        OriginalCollect(self, a2, a3, a4, a5);
        const bool after = track && ((qwords[bit >> 6] >> (bit & 63)) & 1);

        Logger::Log(std::format(
            "Collect: type={} id={} jump={} flagB={} unit={} prop={} bit={} before={} after={}",
            type, id, isJump, flagB, unit, prop, bit, before, after));
    }

    MH_STATUS Install()
    {
        MH_STATUS st = MH_CreateHook(reinterpret_cast<void*>(RegisterAddress),
                                     reinterpret_cast<void*>(&HkRegister),
                                     reinterpret_cast<void**>(&OriginalRegister));
        if (st != MH_OK) return st;
        return MH_CreateHook(reinterpret_cast<void*>(CollectAddress),
                             reinterpret_cast<void*>(&HkCollect),
                             reinterpret_cast<void**>(&OriginalCollect));
    }
}
