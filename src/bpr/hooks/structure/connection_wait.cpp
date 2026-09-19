#include "detours.hpp"
#include "MinHook.h"
#include <cstdint>
#include <iostream>
#include "../../app/app.hpp"

namespace WaitForConnection
{
    constexpr uintptr_t Address = 0x00a4a215;
    constexpr uintptr_t returnAddress = 0x00a4a21A;

    using StateTransitionFn = void (__thiscall*)(
        void* self,
        std::uint32_t value
    );

    constexpr std::uintptr_t StateTransitionAddress = 0x070B0470;

    auto transitionState =  reinterpret_cast<StateTransitionFn>(StateTransitionAddress);


    extern "C" bool __stdcall IsLoadAllowed() {
        return App::Instance != nullptr && App::Instance->State().isConnected();
    }


    __declspec(naked) void Detour()
    {
        __asm
        {
            pushfd
            pushad
            call WaitForConnection::IsLoadAllowed
            test al, al
            jnz allowed

            popad
            popfd

            ret
        allowed:
            popad
            popfd
            push 4
            call dword ptr [transitionState]
            ret
        }
    }

    MH_STATUS Install()
    {
        MH_STATUS status = MH_CreateHook(
            reinterpret_cast<void*>(Address),
            reinterpret_cast<void*>(&Detour),
            nullptr
        );

        return status;
    }
}