#include "detours.hpp"
#include "MinHook.h"
#include <cstdint>
#include <iostream>
#include <windows.h>
#include "../../app/app.hpp"

namespace WndProc
{
    constexpr uintptr_t Address = 0x008FB9D9;

    extern "C" bool __stdcall GUI_WndProc(
        HWND hWnd,
        UINT msg,
        WPARAM wParam,
        LPARAM lParam)
    {
        return  App::WndProc(hWnd, msg, wParam, lParam);
    }

    __declspec(naked) void Hook_WindowProc()
    {
        /*
            LRESULT __stdcall WindowProc(
                HWND hWnd,
                UINT Msg,
                WPARAM wParam,
                LPARAM lParam
            )
        */

        __asm
        {
            // ebp + 0x8: HWND hWnd
            // ebp + 0xC: UINT Msg
            // ebp + 0x10: WPARAM wParam
            // ebp + 0x14: LPARAM lParam

            pushfd
            pushad

            push dword ptr [ebp + 0x14]
            push dword ptr [ebp + 0x10]
            push dword ptr [ebp + 0xC]
            push dword ptr [ebp + 0x8]
            call WndProc::GUI_WndProc

            test al, al
            jnz _end

            popad
            popfd

            // Return from the function without processing the message.
            mov eax, 0
            mov esp, ebp
            pop ebp
            ret 0x10

        _end:
            popad
            popfd

            // Original code.
            push ebx
            mov ebx, dword ptr [ebp + 0xC]
            push esi

            // Jump back.
            push 0x008FB9DE
            ret
        }
    }



    MH_STATUS Install()
    {
        MH_STATUS status = MH_CreateHook(
            reinterpret_cast<void*>(Address),
            reinterpret_cast<void*>(&Hook_WindowProc),
            nullptr
        );

        return status;

    }
}
