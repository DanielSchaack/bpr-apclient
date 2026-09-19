#include "detours.hpp"
#include "MinHook.h"
#include <iostream>
#include <windows.h>
#include "../../app/app.hpp"


namespace RedirectSave
{
    constexpr uintptr_t Address = 0x070c6a90;

    static void* Original = nullptr;

    using FunctionType = void(__thiscall*)(char* outputbuffer, const char* suffix);

    extern "C" const char* __cdecl GetSuffix(const char* suffix)
    {
        if (suffix != nullptr &&
            std::strcmp(suffix, "Save\\") == 0)
        {
            if (App::Instance->State().isDisconnected()){
                std::cout << "Loading with no connection to AP" << std::endl;
                return suffix;
            }
            return "Save\\";
        }

        return suffix;
    }

    __declspec(naked) void Detour()
    {
        __asm
        {
            // Entry:
            //
            // ECX     = outputBuffer
            // [ESP]   = return address
            // [ESP+4] = original suffix

            // Save things we need.
            push ecx
            push dword ptr [esp + 8]   // original suffix

            // Get redirected suffix.
            push dword ptr [esp]       // original suffix
            call GetSuffix
            add esp, 4

            // Stack:
            // [esp]   = original suffix
            // [esp+4] = saved ECX
            // [esp+8] = return address
            // [esp+12]= caller's original suffix

            mov ecx, [esp + 4]         // restore outputBuffer

            // We need Original to see our new suffix as its [ESP+4].
            //
            // Call creates a new return address, so push the desired
            // suffix first.
            push eax
            call dword ptr [Original]

            // Original uses plain RET, so our pushed suffix remains.
            add esp, 4

            // Remove saved original suffix + ECX.
            add esp, 8

            // Caller stack is now exactly as it was when Detour entered.
            ret
        }
    }

    constexpr uintptr_t SaveStringAddress = 0x00DC8030;

    void RedirectSaveDirectory()
    {
        constexpr char replacement[] = "APSV\\";

        DWORD oldProtect{};

        VirtualProtect(
            reinterpret_cast<void*>(SaveStringAddress),
            sizeof(replacement),
            PAGE_READWRITE,
            &oldProtect
        );

        std::memcpy(
            reinterpret_cast<void*>(SaveStringAddress),
            replacement,
            sizeof(replacement)
        );

        VirtualProtect(
            reinterpret_cast<void*>(SaveStringAddress),
            sizeof(replacement),
            oldProtect,
            &oldProtect
        );
    }

    MH_STATUS Install()
    {
        // RedirectSaveDirectory();
        // return MH_OK;
        return MH_CreateHook(
            reinterpret_cast<void*>(Address),
            reinterpret_cast<void*>(&Detour),
            &Original
        );
    }
}