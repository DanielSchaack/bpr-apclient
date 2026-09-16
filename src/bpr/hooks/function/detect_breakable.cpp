#include "detours.hpp"
#include "MinHook.h"
#include <iostream>
#include <windows.h>



namespace DetectBreakable
{
    constexpr uintptr_t Address = 0x032149a3;
    constexpr uintptr_t ReturnAddress = 0x032149a8;
    constexpr uintptr_t jumpAddress = 0x03214a0c;

    static void* OriginalTrampoline = nullptr;
    
    extern "C" __declspec(noinline) void __stdcall
    LogBreakable(std::uint32_t type, std::uint32_t id, std::uint32_t area) noexcept
    {
        std::cout << "Type: " << type << "Id: " << id << "Area: " << area <<std::endl;
    }

    __declspec(naked) void Detour()
    {
        __asm
        {
            pushad
            push ebx
            push dword ptr [ebp+0x0C]
            push dword ptr [ebp+0x08]
            call DetectBreakable::LogBreakable
            
            popad
            cmp ebx, 0x5
            jne skip
            
            jmp ReturnAddress
        skip:
            jmp jumpAddress
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