#include "bpr/core/ap/ap_state.hpp"
#include "bpr/core/logger.hpp"
#include "detours.hpp"
#include <format>
#include <windows.h>
#include <intrin.h>
#include "../../app/app.hpp"

namespace AlwaysWrecked
{
    static constexpr std::uintptr_t Address = 0x00996079;
    static void* OriginalTrampoline = nullptr;

    bool ShouldWreck(bool wrecked) noexcept
    {
        if(App::Instance->State().InDeathTimeout()){
            return true;
        }

        if(wrecked && !g_ProcessingDeathlink && g_CrashType){
            Logger::Log(std::format("Not in timeout while wrecked, try sending deathlink"));
            App::Instance->State().SendDeathLink();
            g_CrashType = 0;
        }

        if(!wrecked && g_ProcessingDeathlink)
        {
            Logger::Log(std::format("No longer wrecked, resetting processing deathlink state"));
            g_ProcessingDeathlink = false;
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
