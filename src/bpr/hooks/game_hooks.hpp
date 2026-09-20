#pragma once


#include <cstdint>
namespace GameHooks
{

    void Init();
    
    constexpr uintptr_t GameModuleAddress  = 0x013FC8E0;
    constexpr uintptr_t StateManagerOffset = 0xB6D478;
    constexpr uintptr_t EventSaveManagerOffset = 0xcb80;
    constexpr uintptr_t GameStateOffset = 0x69B000;
    constexpr uintptr_t GameStateFlagOffset = 0xB6D4C8;

    inline std::uintptr_t GetGameModule() noexcept
    {
        return *reinterpret_cast<std::uintptr_t*>(GameModuleAddress);
    }

    // inline void* GetStateManager() noexcept
    // {
    //     const auto gameModule = GetGameModule();

    //     if (gameModule == 0)
    //         return nullptr;

    //     return reinterpret_cast<void*>(
    //         gameModule + StateManagerOffset
    //     );
    // }

    inline void* GetGameStateManager() noexcept
    {
        const auto gameModule = GetGameModule();

        if (gameModule == 0)
            return nullptr;

        return reinterpret_cast<void*>(
            gameModule + GameStateOffset
        );
    }

    inline void* GetEventSaveManager() noexcept
    {
        const auto gameStateManager =
            reinterpret_cast<std::uintptr_t>(GetGameStateManager());

        if (gameStateManager == 0)
            return nullptr;

        return reinterpret_cast<void*>(
            gameStateManager + EventSaveManagerOffset
        );
    }

    inline uint32_t GetCurrentGameStateFlag() noexcept {
        const auto gameModule = GetGameModule();
        return *reinterpret_cast<uint32_t*>(
            gameModule + GameStateFlagOffset
        );
    }
};