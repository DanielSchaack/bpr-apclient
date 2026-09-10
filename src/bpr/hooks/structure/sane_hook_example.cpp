#include "detours.hpp"
#include "MinHook.h"
#include <cstdint>
#include <windows.h>
#include "../../ui/gui.hpp"


namespace Test
{
    constexpr uintptr_t Address = 0x008FB9D9;

    using FunctionType = LRESULT(__stdcall*)(HWND, UINT, WPARAM, LPARAM);

    FunctionType Original = nullptr;

    LRESULT __stdcall Detour(
        HWND hWnd,
        UINT msg,
        WPARAM wParam,
        LPARAM lParam)
    {
        GUI::WndProc(hWnd, msg, wParam, lParam);
        return Original(hWnd, msg, wParam, lParam);
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