#include "bpr/core/logger.hpp"
#include "detours.hpp"
#include "MinHook.h"
#include <format>
#include <set>
#include <windows.h>
#include "../../app/app.hpp"

namespace DetectBreakable
{
    constexpr uintptr_t Address = 0x032149a3;
    constexpr uintptr_t ReturnAddress = 0x032149a8;
    constexpr uintptr_t jumpAddress = 0x03214a0c;

    static void* OriginalTrampoline = nullptr;

    extern "C" __declspec(noinline) void __stdcall
    LogBreakable(std::uint32_t type, std::uint32_t id, std::uint32_t area) noexcept
    {
        App::Instance->Gui().info_window->AddLogMessage(std::format("{} {} ID: {}", Map::areaIndex[area], Map::typeIndex[type], id));
        Logger::Log(std::format("{} {} ID: {}", Map::areaIndex[area], Map::typeIndex[type], id));
        App::Instance->State().CacheBreakable(area, type);
    }

    constexpr std::uintptr_t RegisterAddress = 0x03214990;   // FUN_03214990
    constexpr std::uintptr_t CollectAddress  = 0x00A14930;   // FUN_00a14930

    inline std::atomic<bool> g_allowCollection{true};        // false = collecting is not counted or saved

    static int CountyOf(std::uint32_t area)                  // mirrors FUN_00bda4a0
    {
        if (area > 0x16)  return 6;
        if (area <= 3)    return 0;
        if (area <= 6)    return 1;
        if (area <= 10)   return 2;
        if (area <= 0xD)  return 3;
        if (area <= 0x11) return 4;
        return 5;
    }

    static std::set<std::pair<std::int32_t, std::uint64_t>> g_reported;

    // ---- Register: permanent progress ----
    using RegisterFn = std::uint32_t (__thiscall*)(void*, std::int32_t, std::uint32_t, std::int32_t, std::int32_t);
    static RegisterFn OriginalRegister = nullptr;

    static std::uint32_t __fastcall HkRegister(void* profile, void*, std::int32_t type,
                                               std::uint32_t idLo, std::int32_t idHi, std::int32_t area)
    {
        const std::uint64_t id = (static_cast<std::uint64_t>(static_cast<std::uint32_t>(idHi)) << 32) | idLo;

        if (g_reported.insert({type, id}).second)
        {
            const int county = CountyOf(area);
            Logger::Log(std::format("{} {} ID: {}", county < 6 ? Map::areaIndex[county] : "?", Map::typeIndex[type], id));
            App::Instance->Gui().info_window->AddLogMessage(std::format("{} {} ID: {}", county < 6 ? Map::areaIndex[county] : "?", Map::typeIndex[type], id));
            App::Instance->State().CacheBreakable(county, type);
        }

        if (!g_allowCollection.load()) return 0;
        return OriginalRegister(profile, type, idLo, idHi, area);
    }

    // ---- Collect: also undo the hit-prop bit it sets inline ----
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

        const bool before = track && ((qwords[bit >> 6] >> (bit & 63)) & 1);
        OriginalCollect(self, a2, a3, a4, a5);
        const bool after = track && ((qwords[bit >> 6] >> (bit & 63)) & 1);

        Logger::Log(std::format(
            "Collect: type={} id={} jump={} flagB={} unit={} prop={} bit={} before={} after={} allow={}",
            type, id, isJump, flagB, unit, prop, bit, before, after, g_allowCollection.load()));


        if (track && !g_allowCollection.load() && !before && after)
        {
            qwords[bit >> 6] &= ~(1ULL << (bit & 63));              // undo only a bit this call set
            Logger::Log("  -> bit cleared (collection blocked)");
        }
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
