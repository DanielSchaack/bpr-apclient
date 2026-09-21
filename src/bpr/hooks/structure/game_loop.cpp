#include "detours.hpp"
#include "MinHook.h"
#include <cstdint>
#include <iostream>
#include <windows.h>
#include "../../app/app.hpp"


namespace GameLoop
{
    constexpr uintptr_t Address = 0x070533c0;

    using FunctionType = void(__fastcall*)(int self);

    FunctionType Original = nullptr;

    void __fastcall Detour(int self)
    {
        App::GameThread();
        Original(self);
        //Code runs after origional
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