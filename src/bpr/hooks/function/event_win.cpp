#include "detours.hpp"
#include <Windows.h>
#include <intrin.h>
#include <iostream>

namespace EventWinLog
{
    constexpr std::uintptr_t Address = 0x06E777F0;
    using FinishFn = void (__thiscall*)(void*, void*, std::uint32_t, void*, std::int32_t);
    static FinishFn Original = nullptr;

    // ECX = manager. EDX is unused by the original __thiscall function.
    // The remaining four arguments stay on the stack, exactly as in the game.
    void __fastcall Detour(void* manager, void*, void* actionQueue,
                          std::uint32_t eventId, void* results, std::int32_t mode)
    {
        const auto position = *reinterpret_cast<const std::int32_t*>(
            static_cast<const unsigned char*>(results) + 0x04);

        Original(manager, actionQueue, eventId, results, mode);
        if (position == 1)
        {
            std::cout << "Event Win: " << eventId << std::endl;
        }
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
