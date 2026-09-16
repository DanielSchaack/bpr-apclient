#include "detours.hpp"
#include "MinHook.h"
#include <cstdint>
#include <iostream>
#include <ostream>
#include <windows.h>

namespace DisableEventStart {


    constexpr uintptr_t Address = 0x00A240C9;
    constexpr uintptr_t SkipAddress = 0x00A243B2;
    static std::uintptr_t ContinueAddress = 0x00A240CF;
    static void* OriginalTrampoline = nullptr;

    extern "C" __declspec(noinline) bool __stdcall
    StreetlightEvent_IsAllowed(std::uint32_t eventId) noexcept
    {
        std::cout << "Trigger Start Id" <<eventId << std::endl;
        // for (const auto allowedId: DisableEventStart::AllowedJunctionIds)
        //     if (allowedId == eventId)
        //         return true;
        return false;
    }


   
    __declspec(naked) void Detour()
    {
        __asm
        {
            pushfd
            pushad


            mov ebx, esp
            sub esp, 528
            and esp, -16
            fxsave [esp]

            push dword ptr [eax+38h]
            call StreetlightEvent_IsAllowed
            test al, al
            jz blocked

            fxrstor [esp]
            mov esp, ebx
            popad
            popfd
    
            mov dword ptr [ebp-30h], eax
            mov eax, dword ptr [eax+38h]
            jmp dword ptr [ContinueAddress]

        blocked:
            fxrstor [esp]
            mov esp, ebx
            popad
            popfd
            jmp dword ptr [SkipAddress]
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