#include "detours.hpp"
#include <Windows.h>
#include <intrin.h>
#include <iostream>
#include "../../app/app.hpp"

namespace LicenseUpgradeLog
{
    constexpr std::uintptr_t Address = 0x00A10530;
    using UpgradeFn = void (__thiscall*)(void*, std::int8_t, void*);
    static UpgradeFn Original = nullptr;

    static int Rank(void* manager)
    {
        return *reinterpret_cast<const std::int8_t*>(
            static_cast<const unsigned char*>(manager) + 0x2C4A8);
    }

    void __fastcall Detour(void* manager, void*, std::int8_t requestedRank, void* actionQueue)
    {
        const int before = Rank(manager);

        Original(manager, requestedRank, actionQueue);
        const int after = Rank(manager);
        //const auto* event = EventWinLog::CurrentWin;
        if (after > before)// && event && event->manager == manager)
        {
            App::Instance->State().SendLocation(1000+after);
            if (
                App::Instance->State().GetSlotData().goalConfig == 0 &&
                after >= App::Instance->State().GetSlotData().licenseGoal
            ){
                App::Instance->State().SendGoal();
            }
        }
    }


    MH_STATUS Install()
    {
        return MH_CreateHook(
            reinterpret_cast<void*>(Address), 
            reinterpret_cast<void*>(&Detour),
            reinterpret_cast<void**>(&Original)
        );
    }
}
