#include "detours.hpp"
#include <Windows.h>
#include <atomic>
#include <cstdint>
#include <intrin.h>
#include <iostream>

namespace DeathLink
{
    using Word = std::uint32_t;
    using Byte = std::uint8_t;
    static constexpr std::uintptr_t CrashAddress = 0x07A383E0;
    static constexpr std::uintptr_t UpdateAddress = 0x00B8B900;
    static constexpr std::uintptr_t ForceAddress = 0x00B7D190;

    using CrashFn = void(__thiscall*)(void*, const float*, Word);
    // Fifteen four-byte stack slots; float arguments are forwarded as raw bits.
    using UpdateFn = void(__thiscall*)(void*, Word, Word, Word, Word, Word,
                                      Word, Word, Word, Word, Word,
                                      Word, Word, Word, Word, Word);
    using ForceFn = void(__thiscall*)(void*, void*, void*, void*, void*,
                                     int, Word, Word);
    static CrashFn OriginalCrash = nullptr;
    static UpdateFn OriginalUpdate = nullptr;
    static std::atomic<bool> Pending{false};

    template<class T> static T& At(void* base, std::uintptr_t offset)
    {
        return *reinterpret_cast<T*>(static_cast<Byte*>(base) + offset);
    }

    static Byte* GetManager()
    {
        auto* game = *reinterpret_cast<Byte**>(0x013FC8E0);
        if (!game || At<Word>(game, 0xB6D464) != 1)
            return nullptr;
        return game + 0x1AB140;
    }

    static void* GetPlayerCar(void* manager)
    {
        if (!manager)
            return nullptr;
        const auto index = At<int>(manager, 0x3616C);
        if (index < 0 || index >= 8 ||
            !(At<Word>(manager, 0x18100) & (1u << index)))
            return nullptr;
        return At<void*>(manager, 0x17E20 + index * 4);
    }

    static void __fastcall CrashDetour(void* car, void*, const float* scale, Word kind)
    {
        const bool local = car == GetPlayerCar(GetManager());
        const bool wasCrashing = At<Byte>(car, 0x1170) != 0;

        OriginalCrash(car, scale, kind);

        if (local && !wasCrashing && At<Byte>(car, 0x1170))
        {
            std::cout << "[PlayerCrash] Local player entered crash state (kind=%u).\n" << static_cast<unsigned>(At<Byte>(car, 0x1171)) << std::endl;
        }
    }

    static void TryCrash(void* manager, Word managerOutput, Word vehicleOutput, Word worldEntity)
    {
        if (manager != GetManager() || !managerOutput || !vehicleOutput)
            return;
        auto* car = GetPlayerCar(manager);
        if (!car || At<Byte>(car, 0x1170) || At<int>(car, 0x1190) == 2)
            return;

        const auto index = At<int>(manager, 0x3616C);
        // The game's own forced-crash branch clears these protection flags.
        // Limit that override to this call, then restore the previous settings.
        auto& stopCrashing = At<Byte>(manager, 0x361B2);
        auto& invulnerable = At<std::uint16_t>(manager, 0x176E2 + index * 0xF0);
        const auto oldStop = stopCrashing;
        const auto oldInvulnerable = invulnerable;
        stopCrashing = 0;
        invulnerable = 0;

        reinterpret_cast<ForceFn>(ForceAddress)(manager,
            nullptr, reinterpret_cast<void*>(managerOutput),
            reinterpret_cast<void*>(vehicleOutput), nullptr,
            index, worldEntity, 5); // Kind 1: used by the normal wall-impact path.

        invulnerable = oldInvulnerable;
        stopCrashing = oldStop;
    }

    static void __fastcall UpdateDetour(void* manager, void*,
        Word a1, Word a2, Word a3, Word a4, Word a5,
        Word a6, Word a7, Word a8, Word a9, Word a10,
        Word a11, Word a12, Word a13, Word a14, Word a15)
    {
        if (Pending.exchange(false))
            TryCrash(manager, a9, a7, a13);
        OriginalUpdate(manager, a1, a2, a3, a4, a5, a6, a7, a8,
                       a9, a10, a11, a12, a13, a14, a15);
    }

    bool KillPlayer() noexcept
    {
        Pending.store(true);
        return true;
    }

    MH_STATUS Install()
    {
        auto* crash = reinterpret_cast<void*>(CrashAddress);
        auto* update = reinterpret_cast<void*>(UpdateAddress);
        auto status = MH_CreateHook(crash, reinterpret_cast<void*>(&CrashDetour),
                                   reinterpret_cast<void**>(&OriginalCrash));
        if (status != MH_OK)
            return status;
        status = MH_CreateHook(update, reinterpret_cast<void*>(&UpdateDetour),
                               reinterpret_cast<void**>(&OriginalUpdate));
        if (status != MH_OK)
        {
            MH_RemoveHook(crash);
            return status;
        }
        return MH_OK;
    }
}
