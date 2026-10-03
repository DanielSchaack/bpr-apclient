#include "MinHook.h"
#include "bpr/app/app.hpp"
#include "bpr/core/logger.hpp"
#include <format>

namespace DetectTakedown
{
    constexpr std::uintptr_t Address = 0x00A0FCC0;

    using TakedownFn = void (__thiscall*)(void* mgr, std::uint32_t a2, std::int32_t type,
                                          std::uint32_t a4, std::uint32_t a5,
                                          std::uint32_t flagA, std::uint32_t flagB);
    static TakedownFn Original = nullptr;

    static std::int32_t Total(void* mgr)
    {
        return *reinterpret_cast<std::int32_t*>(static_cast<std::uint8_t*>(mgr) + 0x338);
    }
    static std::int32_t PerType(void* mgr, int type)
    {
        return reinterpret_cast<std::int32_t*>(static_cast<std::uint8_t*>(mgr) + 0x340)[type];
    }

    static void __fastcall Detour(void* mgr, void*, std::uint32_t a2, std::int32_t type,
                                  std::uint32_t a4, std::uint32_t a5,
                                  std::uint32_t flagA, std::uint32_t flagB)
    {
        const std::int32_t before = Total(mgr);

        Original(mgr, a2, type, a4, a5, flagA, flagB);

        const std::int32_t after = Total(mgr);
        if (after > before && type >= 0 && type < 13)
        {
            Logger::Log(std::format("Takedown: type {} | total {} | count of this type {}", type, after, PerType(mgr, type)));
            App::Instance->Gui().info_window->AddLogMessage(std::format("Takedown: type {} | total {} | count of this type {}", type, after, PerType(mgr, type)));
            // TODO: App::Instance->State().SendLocation(...);
        }
    }

    MH_STATUS Install()
    {
        return MH_CreateHook(reinterpret_cast<void*>(Address),
                             reinterpret_cast<void*>(&Detour),
                             reinterpret_cast<void**>(&Original));
    }
}
