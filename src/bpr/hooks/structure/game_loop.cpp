#include "detours.hpp"
#include "MinHook.h"
#include <cstdint>
#include <iostream>
#include <windows.h>
#include "../../app/app.hpp"


namespace GameLoop
{
    constexpr uintptr_t GamePreworldUpdateAddress = 0x00A253F0;

    using FunctionType = void(__thiscall*)(
        void* self,
        void* gameEventQueue,
        void* gameActionQueue,
        void* arg3,
        void* arg4
    );

    static FunctionType Original = nullptr;

    void __fastcall Detour(
        void* self,
        void* /* edx */,
        void* gameEventQueue,
        void* gameActionQueue,
        void* arg3,
        void* arg4)
    {
        App::GameThread(gameActionQueue);

        Original(
            self,
            gameEventQueue,
            gameActionQueue,
            arg3,
            arg4
        );
    }
    MH_STATUS Install()
    {
        MH_STATUS status = MH_CreateHook(
            reinterpret_cast<void*>(GamePreworldUpdateAddress),
            reinterpret_cast<void*>(&Detour),
            reinterpret_cast<void**>(&Original)
        );
        return status;
    }
}