#pragma once

#include "MinHook.h"

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
namespace WndProc
{
    MH_STATUS Install();
}

namespace Test
{
    MH_STATUS Install();
}

namespace Render
{
    MH_STATUS Install();
}

namespace GameLoop {
    MH_STATUS Install();
}

namespace RedirectSave {
    MH_STATUS Install();
}

namespace WaitForConnection {
    MH_STATUS Install();
}