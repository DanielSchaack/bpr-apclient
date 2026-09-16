#include "detours.hpp"
#include "MinHook.h"
#include <cstdint>
#include <windows.h>
#include "../../app/app.hpp"


namespace Render
{
    constexpr uintptr_t Address = 0x0817E440;

    using FunctionType = void(__thiscall*)(void* self);

    FunctionType Original = nullptr;

    void __stdcall Detour(void* self, void*)
    {
        App::Render();
        Original(self);
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