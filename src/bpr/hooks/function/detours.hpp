#pragma once

#include "MinHook.h"
#include <array>
#include <cstdint>
#include <windows.h>

namespace NOP
{
    inline void NopInstructions(void* address, size_t size) {
        DWORD oldProtect;
        VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
        memset(address, 0x90, size); // 0x90 = NOP
        VirtualProtect(address, size, oldProtect, &oldProtect);
        FlushInstructionCache(GetCurrentProcess(), address, size);
    }
}

namespace logEvent
{
    MH_STATUS Install();
}

namespace DisableTrigger
{

    MH_STATUS Install();
}

namespace DisableEventStart
{
    MH_STATUS Install();
}

namespace DetectBreakable
{
    MH_STATUS Install();
}