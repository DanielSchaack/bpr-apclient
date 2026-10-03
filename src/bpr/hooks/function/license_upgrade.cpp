#include "bpr/core/logger.hpp"
#include "detours.hpp"
#include <windows.h>
#include <intrin.h>
#include "../../app/app.hpp"

namespace LicenseUpgradeLog
{
    constexpr std::uintptr_t Address = 0x00A10530;
    static void* Original = nullptr;   // MinHook trampoline

    // Live profile rank = manager + 0x210 (profile + 0xA0)
    static int __cdecl GetRank(void* manager)
    {
        return *reinterpret_cast<const std::int8_t*>(
            static_cast<const unsigned char*>(manager) + 0x210);
    }

    static void __cdecl OnAfter(void* manager, int before)
    {
        const int after = GetRank(manager);
        Logger::Log(std::format("License Upgrade: rank {} -> {}", before, after));
        App::Instance->Gui().info_window->AddLogMessage(std::format("License Upgrade: rank {} -> {}", before, after));

        if (after > before)
        {
            App::Instance->State().SendLocation(1000 + after);
            if (App::Instance->State().GetSlotData().goalConfig == 0 &&
                after >= App::Instance->State().GetSlotData().licenseGoal)
            {
                App::Instance->State().SendGoal();
            }
        }
    }

    __declspec(naked) void Detour()
    {
        __asm
        {
            push ebp
            mov  ebp, esp
            sub  esp, 8                    // [ebp-4] = manager, [ebp-8] = rank before
            mov  [ebp-4], ecx

            pushad                         // keep all registers intact around the helper
            push ecx
            call GetRank
            add  esp, 4
            mov  [ebp-8], eax
            popad

            push dword ptr [ebp+12]        // arg 2 (action queue)
            push dword ptr [ebp+8]         // arg 1 (rank, full dword slot)
            call dword ptr [Original]      // callee pops both args

            pushad                         // preserve EAX/EDX return values
            push dword ptr [ebp-8]
            push dword ptr [ebp-4]
            call OnAfter
            add  esp, 8
            popad

            mov  esp, ebp
            pop  ebp
            ret  8
        }
    }

    MH_STATUS Install()
    {
        return MH_CreateHook(
            reinterpret_cast<void*>(Address),
            reinterpret_cast<void*>(&Detour),
            &Original);
    }
}
