#include "detours.hpp"
#include "MinHook.h"
#include <cstdint>
#include <iostream>
#include <ostream>
#include <windows.h>

namespace logEvent
{
    constexpr uintptr_t Address = 0x06e5d2a0;
    

    using FunctionType = LRESULT(__thiscall*)(int, int);

    FunctionType Original = nullptr;

    LRESULT __thiscall Detour(
        int param1,
        int param2)
    {
        std::cout << "TEST Param1: " << param1 << "Param2: " << param2 << std::endl;
        return Original(param1, param2);
    }

    void Install()
    {
        MH_CreateHook(
            reinterpret_cast<void*>(Address),
            reinterpret_cast<void*>(&Detour),
            reinterpret_cast<void**>(&Original)
        );
    }
}