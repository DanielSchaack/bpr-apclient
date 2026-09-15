#include "detours.hpp"
#include "MinHook.h"
#include <cstdint>
#include <iostream>
#include <ostream>
#include <windows.h>

namespace DisableTrigger
{

    
    constexpr uintptr_t Address = 0x00a21d73;
    constexpr uintptr_t returnAdress = 0x00a21d73 + 5;
    constexpr uintptr_t endAddress = 0x00A223FA;


    extern "C" bool __stdcall Test_Access(
        int obj_id)
    {
        std::cout << "OBject Id: " << obj_id << std::endl;
        if (false){
            return true;
        }
        return false;
    }


     __declspec(naked) void Detour()
    {
        __asm
        {
            pushad
            push dword ptr [ebp-0x14]
            call DisableTrigger::Test_Access
            test al, al
            je blocked
            popad
            mov esi, eax
            mov [ebp-0x14], esi
            jmp returnAdress

            blocked:
            popad
            jmp endAddress
        }
    }

    void Install()
    {
        MH_CreateHook(
            reinterpret_cast<void*>(Address),
            reinterpret_cast<void*>(&Detour),
            nullptr
        );
    }
}