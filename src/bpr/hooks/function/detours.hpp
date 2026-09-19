#pragma once

#include "MinHook.h"
#include <cstdint>
#include <windows.h>
#include "../game_hooks.hpp"

namespace Helper
{
    inline void NopInstructions(void* address, size_t size) {
        DWORD oldProtect;
        VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
        memset(address, 0x90, size); // 0x90 = NOP
        VirtualProtect(address, size, oldProtect, &oldProtect);
        FlushInstructionCache(GetCurrentProcess(), address, size);
    }
    inline uintptr_t gameModule = 0x013FC8E0;
    inline uintptr_t gameStateModule = *reinterpret_cast<uintptr_t*>(gameModule) + 0x69B000;
}

namespace logEvent
{
    MH_STATUS Install();
}

namespace DisableTrigger
{

    MH_STATUS Install();
}

namespace DisableEventStart
{
    MH_STATUS Install();
}

namespace DetectBreakable
{
    MH_STATUS Install();
}

namespace CarUnlockControl
{
    bool AddCar(std::uint64_t vehicleId);
    MH_STATUS Install();
}

namespace LicenseUpgradeLog
{
    MH_STATUS Install();
}

namespace EventWinLog
{
    MH_STATUS Install();
}

namespace DeathLink
{
    // Call after MH_Initialize(), during mod startup.
    MH_STATUS Install();

    // Queue one crash for the next vehicle physics update.
    // True means queued, not that a crash/cutscene has already happened.
    // Repeated requests before that update are coalesced.
    bool KillPlayer() noexcept;
}

namespace EnableEvent
{
    bool EnableEvent(uint32_t event_id) noexcept;
    bool IsEventEnabled(uint32_t  event_id) noexcept; 
}