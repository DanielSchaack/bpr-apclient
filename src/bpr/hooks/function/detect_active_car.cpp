#include "MinHook.h"
#include "bpr/core/logger.hpp"
#include "bpr/hooks/function/detours.hpp"
#include "bpr/hooks/game_hooks.hpp"      // GameHooks::GetProfile(); adjust to your header's path
#include <atomic>
#include <format>

namespace DetectActiveCar
{
    namespace
    {
        constexpr std::uintptr_t HookAddress = 0x00A10333;   // mov [esi+ecx*8+0x1C0], ebx
        constexpr std::uintptr_t ResumeAddress = 0x00A10341; // right after the second mov (7 + 7 bytes)

        void* g_unusedTrampoline = nullptr;                  // never called, the detour replays the originals
        std::atomic<std::int32_t> g_activeSlot{-1};
    }

    // ecx at the hook is the slot being written (0 = car, 1 = bike)
    extern "C" __declspec(noinline) void __stdcall ActiveCarSlotWritten(std::uint32_t slot) noexcept
    {
        if (slot > 1)
        {
            Logger::Log(std::format("DetectActiveCar: unexpected slot {}", slot));
            return;
        }
        g_activeSlot = static_cast<std::int32_t>(slot);
    }

    __declspec(naked) void Detour()
    {
        __asm
        {
            pushfd
            pushad
            push ecx                              // slot
            call DetectActiveCar::ActiveCarSlotWritten
            popad
            popfd
            mov  [esi+ecx*8+0x1C0], ebx           // replayed original instructions
            mov  [esi+ecx*8+0x1C4], edi
            jmp  ResumeAddress
        }
    }

    Slot GetActiveSlot() noexcept
    {
        return static_cast<Slot>(g_activeSlot.load());
    }

    bool IsBikeActive() noexcept
    {
        return GetActiveSlot() == Slot::Bike;
    }

    std::uint64_t GetSlotId(Slot slot) noexcept
    {
        const std::uint8_t* profile = GameHooks::GetProfile();
        if (!profile || (slot != Slot::Car && slot != Slot::Bike))
            return 0;
        const auto offset = (slot == Slot::Car) ? SpawnCarIdOffset : SpawnBikeIdOffset;
        return *reinterpret_cast<const std::uint64_t*>(profile + offset);
    }

    std::uint64_t GetActiveVehicleId() noexcept
    {
        const Slot slot = GetActiveSlot();
        return GetSlotId(slot == Slot::Unknown ? Slot::Car : slot);
    }

    const VehicleInfo* GetActiveVehicle() noexcept
    {
        const std::uint64_t id = GetActiveVehicleId();
        if (!id)
            return nullptr;
        if (const VehicleInfo* v = FindVehicleByID(id))
            return v;
        return FindVehicleByLiveryID(id);
    }

    MH_STATUS Install()
    {
        return MH_CreateHook(reinterpret_cast<void*>(HookAddress),
                             reinterpret_cast<void*>(&Detour),
                             &g_unusedTrampoline);
    }
}
