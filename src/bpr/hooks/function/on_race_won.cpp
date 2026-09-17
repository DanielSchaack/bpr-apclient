#include "MinHook.h"
#include <cstdint>
#include "../../app/app.hpp"

namespace EventResultHook
{
    constexpr std::uintptr_t Address = 0x06E777F0;

    // Argument names remain provisional.
    using Function = void(__thiscall*)(
        void* manager,
        std::uint32_t arg1,
        std::uint32_t eventid,
        void* result,
        std::uint32_t arg4
    );

    static Function Original = nullptr;

    void __fastcall Detour(
        void* manager,       // Original ECX / this pointer
        void* /*unusedEdx*/, // Fastcall placeholder; not a game argument
        std::uint32_t arg1,
        std::uint32_t eventid,
        void* result,
        std::uint32_t arg4)
    {
        App::Instance->State().SendLocation(eventid);

        Original(manager, arg1, eventid, result, arg4);

        // Your code after the original runs.
    }

    // Call after MH_Initialize().
    MH_STATUS Install()
    {
        auto* target = reinterpret_cast<void*>(Address);

        auto status = MH_CreateHook(
            target,
            reinterpret_cast<void*>(&Detour),
            reinterpret_cast<void**>(&Original)
        );

        if (status != MH_OK)
            return status;

        status = MH_EnableHook(target);
        if (status != MH_OK)
        {
            MH_RemoveHook(target);
            Original = nullptr;
        }

        return status;
    }
}