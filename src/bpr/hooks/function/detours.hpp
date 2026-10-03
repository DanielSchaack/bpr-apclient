#pragma once

#include "MinHook.h"
#include "bpr/core/vehicles.hpp"
#include <cstdint>
#include <windows.h>

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

    inline bool WriteBytes(std::uintptr_t address, const void* bytes, size_t size)
    {
        void* dst = reinterpret_cast<void*>(address);
        DWORD oldProtect;

        if (!VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect))
            return false;

        memcpy(dst, bytes, size);

        VirtualProtect(dst, size, oldProtect, &oldProtect);
        FlushInstructionCache(GetCurrentProcess(), dst, size);
        return true;
    }
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
    bool RemoveCar(std::uint64_t vehicleId);
    bool AddLiveries(std::uint64_t archipelagoLiveryId);
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
    MH_STATUS Install();

    bool KillPlayer() noexcept;
}

namespace EnableEvent
{
    bool EnableEvent(uint32_t event_id) noexcept;
    bool IsEventEnabled(uint32_t  event_id) noexcept; 
    bool IsValidEvent(uint32_t  event_id) noexcept; 
}

namespace AlwaysWrecked
{
    MH_STATUS Install();
}

namespace CarAchievements
{
    bool Install();
}

namespace DetectRoadRules
{
    MH_STATUS Install();
}

namespace DetectTakedown
{
    MH_STATUS Install();
}

namespace DetectDriveThru
{
    MH_STATUS Install();
}

namespace CrashLog
{
    void Install();
}

namespace DetectActiveCar
{
    constexpr std::uintptr_t SpawnCarIdOffset  = 0x50;   // slot 0: last/current car,  8 bytes
    constexpr std::uintptr_t SpawnBikeIdOffset = 0x58;   // slot 1: last/current bike, 8 bytes

    enum class Slot : std::int32_t { Unknown = -1, Car = 0, Bike = 1 };

    MH_STATUS Install();                                  // creates the hook; enable it like your others

    Slot          GetActiveSlot() noexcept;               // slot the game wrote last, Unknown until the first write
    bool          IsBikeActive() noexcept;
    std::uint64_t GetSlotId(Slot slot) noexcept;          // raw ID stored in a slot, 0 if the profile isn't live
    std::uint64_t GetActiveVehicleId() noexcept;          // active slot's ID; falls back to the car slot
    const VehicleInfo* GetActiveVehicle() noexcept;       // table entry (ID or livery ID), nullptr if unknown

    void Reset() noexcept;                                // forget the active slot, e.g. on a new session
}
