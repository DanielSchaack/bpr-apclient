#pragma once


#include "bpr/core/logger.hpp"
#include "bpr/core/vehicles.hpp"
#include "bpr/hooks/function/detours.hpp"
#include <cstdint>
#include <format>
namespace GameHooks
{

    void Init();

    constexpr uintptr_t GameModuleAddress  = 0x013FC8E0;
    constexpr uintptr_t StateManagerOffset = 0xB6D478;
    constexpr uintptr_t EventSaveManagerOffset = 0xcb80;
    constexpr uintptr_t GameStateOffset = 0x69B000;
    constexpr uintptr_t GameStateFlagOffset = 0xB6D4C8;
    constexpr uintptr_t VehicleArrayOffset = 0x12980;
    constexpr uintptr_t VehicleStride      = 0x4180;
    constexpr uintptr_t DeformationOffset = 0x8AC;
    constexpr uintptr_t BoostLevelOffset  = 0x40754;

    // ---- profile (manager + 0x170) ----
    constexpr uintptr_t ProgressionManagerOffset = 0x6A7A10;
    constexpr uintptr_t ProfileOffset            = ProgressionManagerOffset + 0x170;   // 0x6A7B80
    constexpr uintptr_t GameStateReadyOffset     = 0xB6D464;                            // == 1 when the profile is live
    constexpr uintptr_t SpawnCarIdOffset         = 0x50;     // mSpawnCarId, 8 bytes
    constexpr uintptr_t CarCountOffset           = 0x2A0;
    constexpr uintptr_t CarArrayOffset           = 0x2B8;    // maCars, from the save loader
    constexpr uintptr_t CarRecordSize            = 0x18;
    constexpr int       MaxGarageCars            = 512;

    inline std::uintptr_t GetGameModule() noexcept
    {
        return *reinterpret_cast<std::uintptr_t*>(GameModuleAddress);
    }

    inline void* GetGameStateManager() noexcept
    {
        const auto gameModule = GetGameModule();
        if (gameModule == 0)
            return nullptr;
        return reinterpret_cast<void*>(gameModule + GameStateOffset);
    }

    inline void* GetEventSaveManager() noexcept
    {
        const auto gameStateManager = reinterpret_cast<std::uintptr_t>(GetGameStateManager());
        if (gameStateManager == 0)
            return nullptr;
        return reinterpret_cast<void*>(gameStateManager + EventSaveManagerOffset);
    }

    inline uint32_t GetCurrentGameStateFlag() noexcept
    {
        const auto gameModule = GetGameModule();
        if (!gameModule)
            return 0;
        return *reinterpret_cast<uint32_t*>(gameModule + GameStateFlagOffset);
    }

    bool isInGame() noexcept;

    inline std::uint8_t* GetProfile() noexcept
    {
        const auto gameModule = GetGameModule();
        if (!gameModule)
            return nullptr;
        if (*reinterpret_cast<std::uint32_t*>(gameModule + GameStateReadyOffset) != 1)
            return nullptr;
        return reinterpret_cast<std::uint8_t*>(gameModule + ProfileOffset);
    }

    inline int32_t GetPlayerCarIndex()
    {
        const auto gameModule = GetGameModule();
        if (!gameModule)
            return 0;
        return *reinterpret_cast<std::int32_t*>(gameModule + 0x40C28);
    }

    inline uintptr_t GetRaceVehicleAddress(uintptr_t gameModule, std::int32_t vehicleIndex)
    {
        return gameModule + VehicleArrayOffset + static_cast<uintptr_t>(vehicleIndex) * VehicleStride;
    }

    inline uintptr_t GetRaceVehicleAddress()
    {
        return GetRaceVehicleAddress(GetGameModule(), GetPlayerCarIndex());
    }

    inline float GetCarDeformation()
    {
        const uintptr_t vehicle = GetRaceVehicleAddress();
        if (!vehicle)
            return 0.0f;
        return *reinterpret_cast<float*>(vehicle + DeformationOffset);
    }

    inline std::int32_t GetBoostLevel()
    {
        const uintptr_t gameModule = GetGameModule();
        if (!gameModule)
            return 0;
        return *reinterpret_cast<std::int32_t*>(gameModule + BoostLevelOffset);
    }

    inline void PrintCurrentCarInformation()
    {
        const uintptr_t vehicle = GetRaceVehicleAddress();
        if (!vehicle)
            return;

        Logger::Log(std::format("activeRaceVehicle: 0x{:X}", vehicle));
        Logger::Log(std::format("deformation: {}", GetCarDeformation()));
        Logger::Log(std::format("boostLevel: {}", GetBoostLevel()));

        const auto slot = DetectActiveCar::GetActiveSlot();
        const std::uint64_t id = DetectActiveCar::GetActiveVehicleId();
        Logger::Log(std::format("active slot {} | vehicle id {:#018x}", static_cast<int>(slot), id));

        if (const VehicleInfo* v = DetectActiveCar::GetActiveVehicle())
            Logger::Log(std::format("table: '{}' category {} unlock {}", v->name,
                                    static_cast<int>(v->category), static_cast<int>(v->unlock_type)));
        else
            Logger::Log("table: id not found");
    }
};
