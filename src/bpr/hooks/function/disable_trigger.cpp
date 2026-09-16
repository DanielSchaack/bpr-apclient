#include "detours.hpp"
#include "MinHook.h"
#include <cstdint>
#include <iostream>
#include <ostream>
#include <windows.h>
#include "../../app/app.hpp"

namespace DisableTrigger {


constexpr uintptr_t Address = 0x00a21d73;
constexpr uintptr_t returnAdress = 0x00a21d73 + 5;
constexpr uintptr_t endAddress = 0x00A223FA;

extern "C" bool __stdcall IsJunctionAllowed(std::uint32_t eventID) {

  std::cout << "UI Id" <<eventID << std::endl;
//   for (const auto allowedId : App::Instance->State().getRecievedItems()) {
//     if (allowedId == eventID)
//       return true;
//   }

  return false;
}


     __declspec(naked) void Detour()
    {
        __asm
        {
            pushad
            push dword ptr [ebp-0x10]
            call DisableTrigger::IsJunctionAllowed
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

    MH_STATUS Install()
    {
        return MH_CreateHook(
            reinterpret_cast<void*>(Address),
            reinterpret_cast<void*>(&Detour),
            nullptr
        );
    }
}