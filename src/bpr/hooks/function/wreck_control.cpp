#include "detours.hpp"
#include <Windows.h>
#include <intrin.h>
#include <iostream>
#include "../../app/app.hpp"

namespace AlwaysWrecked
{
    static constexpr std::uintptr_t Address = 0x00996079;
    static void* OriginalTrampoline = nullptr;

    bool ShouldWreck(bool wrecked) noexcept
    {
        if( App::Instance->State().InDeathTimeout()){
            return true;
        }

        if(wrecked){
            App::Instance->State().SendDeathLink();
        }
        return false;
    }

    __declspec(naked) void Detour()
    {
        __asm
        {
            pushfd
            pushad
            push [esi+852h]
            call ShouldWreck
            add esp, 4
            test al, al

            popad

            je disabled

            popfd

            // ESI = ActiveRaceCar*
            mov byte ptr [esi+852h], 1
            mov byte ptr [esi+849h], 0
            mov byte ptr [esi+800h], 0
            mov dword ptr [esi+7FCh], 0

            jmp dword ptr [OriginalTrampoline]

        disabled:
            popfd
            jmp dword ptr [OriginalTrampoline]
        }
    }

    MH_STATUS Install()
    {

        auto status = MH_CreateHook(
            reinterpret_cast<void*>(Address),
            reinterpret_cast<void*>(&Detour),
            &OriginalTrampoline
        );


        return status;
    }
}