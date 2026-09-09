#include <windows.h>
#include <iostream>

extern "C" __declspec(dllexport)
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            // Code here runs when the DLL is loaded into the process memory
            std::cout << "DLL Loaded Successfully!\n";
            break;

        case DLL_THREAD_ATTACH:
            // Code here runs when a new thread is created in the process
            break;

        case DLL_THREAD_DETACH:
            // Code here runs when a thread exits cleanly
            break;

        case DLL_PROCESS_DETACH:
            // Code here runs when the DLL is unloaded from memory
            std::cout << "DLL Unloaded!\n";
            break;
    }
    return TRUE; // Successful initialization
}