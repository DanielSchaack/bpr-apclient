#pragma once


#include <cstdint>
#include <iostream>
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

    bool isInGame() noexcept;

    inline int32_t GetPlayerCarIndex(){
        const auto gameModule = GetGameModule();

        if (!gameModule)
            return 0;

        return *reinterpret_cast<std::int32_t*>(
                gameModule + 0x40C28
            );

    }

    inline void PrintCurrentDeformation()
    {

        uintptr_t gameModule =
            *reinterpret_cast<uintptr_t*>(GameModuleAddress);

        if (!gameModule)
            return;

        const std::int32_t playerVehicleIndex =
            *reinterpret_cast<std::int32_t*>(
                gameModule + 0x40C28
            );

        uintptr_t activeRaceVehicle =
            gameModule
            + 0x12980
            + playerVehicleIndex * 0x4180;
        
        uintptr_t carptr = gameModule
            + 0x12980
            + playerVehicleIndex * 0x4180;
        const float deformation =
            *reinterpret_cast<float*>(
                activeRaceVehicle + 0x8AC
            );

        std::cout << "Deformation: " << deformation << " ptr: " << carptr << std::endl;
    }

    inline void PrintCurrentBoostLevel()
    {
        constexpr std::uintptr_t GameModulePtrAddress = 0x013FC8E0;

        auto* gameModule =
            *reinterpret_cast<std::uint8_t**>(GameModulePtrAddress);

        if (!gameModule)
            return;

        const std::int32_t boostLevel =
            *reinterpret_cast<std::int32_t*>(gameModule + 0x40754);

        std::cout << "Boost Level: " << boostLevel << '\n';
    }
};