#include <windows.h>
#include <iostream>
#include "../hooks/game_hooks.hpp"
#include "app.hpp"

DWORD WINAPI InitializeThread(LPVOID)
{
    AllocConsole();
    FILE* file;
    freopen_s(&file, "CONOUT$", "w", stdout);
    HWND hwndConsole = GetConsoleWindow();
    ShowWindow(hwndConsole, SW_SHOW);
    std::cout << "DLL Loaded Successfully! WOOOOOOO" << std::endl;

    while (true)
    {
        uintptr_t gameModule = *reinterpret_cast<uintptr_t*>(0x013FC8E0);
        if (gameModule != 0)
        {
            int32_t gameUpdateStage = *(reinterpret_cast<int32_t*>(gameModule + 0xB6D464));
            if (gameUpdateStage == 1)
                break;
        }
        Sleep(1000);
    }
    std::cout << "Game initialized. Initializing AP mod" << std::endl;
    GameHooks::Init();

    new App();

    return 0;
}

extern "C" __declspec(dllexport)
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            HANDLE modThread = CreateThread(
                nullptr,
                0,
                InitializeThread,
                nullptr,
                0,
                nullptr
            );
            if (modThread)
                CloseHandle(modThread);
            break;
        }

        case DLL_THREAD_ATTACH:
            // Code here runs when a new thread is created in the process
            break;

        case DLL_THREAD_DETACH:
            // Code here runs when a thread exits cleanly
            break;

        case DLL_PROCESS_DETACH:
            // Code here runs when the DLL is unloaded from memory
            std::cout << "DLL Unloaded!\n";
            FreeConsole();
            break;
    }
    return TRUE; // Successful initialization
}