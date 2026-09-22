#include "bpr/core/logger.hpp"
#include "detours.hpp"
#include "MinHook.h"
#include <format>
#include <iostream>
#include <string>
#include <windows.h>
#include "../../app/app.hpp"



namespace DetectBreakable
{
    constexpr uintptr_t Address = 0x032149a3;
    constexpr uintptr_t ReturnAddress = 0x032149a8;
    constexpr uintptr_t jumpAddress = 0x03214a0c;

    static void* OriginalTrampoline = nullptr;

    const std::vector<std::string> areaIndex = {
        "Palm Bay Heights",
        "Silver Lake",
        "Harbor Town",
        "White Mountain",
        "Downtown Paradise",
        "Big Surf Island"
    };

    const std::vector<std::string> typeIndex = {
        "Super Jump",
        "Smash",
        "Billboard"
    };

    extern "C" __declspec(noinline) void __stdcall
    LogBreakable(std::uint32_t type, std::uint32_t id, std::uint32_t area) noexcept
    {
        App::Instance->Gui().info_window->AddLogMessage(std::format("{} {} ID: {}", areaIndex[area], typeIndex[type], id));
        Logger::Log(std::format("{} {} ID: {}", areaIndex[area], typeIndex[type], id));
        int64_t loc_id = 10000 + (1000 * area) + (100 * type);
        App::Instance->State().CacheBreakable(loc_id, area);
    }

    __declspec(naked) void Detour()
    {
        __asm
        {
            pushad
            push ebx
            push dword ptr [ebp+0x0C]
            push dword ptr [ebp+0x08]
            call DetectBreakable::LogBreakable
            
            popad
            cmp ebx, 0x5
            jne skip
            
            jmp ReturnAddress
        skip:
            jmp jumpAddress
        }
    }

    MH_STATUS Install()
    {
        return MH_CreateHook(
            reinterpret_cast<void*>(Address),
            reinterpret_cast<void*>(&Detour),
            &OriginalTrampoline
        );
    }
}