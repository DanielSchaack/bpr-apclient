#pragma once

#include "MinHook.h"
#include <cstdint>

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

namespace GameActions {
    bool GameActionQueue_AddGameAction(void* gameActionQueue, const void* gameAction, int32_t gameActionID, int32_t gameActionSize);

    enum class VehicleType
    {
        Invalid = -1,
        Car = 0,
        Motorbike = 1,
        Plane = 2,
    };

    enum class BoostType
    {
        Speed = 0,
        Aggression = 1,
        Stunt = 2,
        None = 3,
        Locked = 4,
    };

    /*
        enum BrnWorld::CarControl
    */
    enum class PlayerVehicleDriver
    {
        None = 0,
        Player = 1,
        AI = 2,
    };

    /*
        enum BrnPhysics::Deformation::DeformationResetType
    */
    enum class DeformationType
    {
        None = -1,
        Event = 0,
        VehicleSelect = 1,
    };

    /*
        enum BrnGameState::GameStateModuleIO::ResetPlayerCarAction::CarSelectType
    */
    enum class VehicleSelectType
    {
        DontDrop = 0,
        DropNormal = 1,
        DropShutdown = 2,
    };

    struct GameAction_ResetPlayerVehicle
    {
        // BrnGameState::GameStateModuleIO::EGameActionType::E_ACTION_RESET_PLAYER_CAR
        static constexpr int32_t ID = 0;

        alignas(16) float Position[4];
        alignas(16) float Direction[4];
        uint64_t VehicleID;
        uint64_t WheelID;
        int32_t PlayerScoringIndex;
        float DeformationAmount;
        DeformationType DeformationType;
        VehicleSelectType VehicleSelectType;
        VehicleType VehicleType;
        bool InVehicleSelect;
        bool VehicleSelectDontStreamAudio;
        bool ResetPlayerCamera;
        bool KeepResetSection;
        bool Unknown1;
        int32_t Unknown2;
    };

    struct GameAction_SetBoost
    {
        // BrnGameState::GameStateModuleIO::EGameActionType::E_ACTION_SET_BOOST
        static constexpr int32_t ID = 197;

        int32_t ActiveRaceVehicleIndex;
        struct
        {
            bool InfiniteBoost : 1;
            bool BoostAmount : 1;
            bool BoostSegments : 1;
            bool BoostEarningEnabled : 1;
            bool BoostMessagesEnabled : 1;
            bool EmptyBoost : 1;
        } Flags;
        float BoostAmount;
        int32_t BoostSegments;
        bool InfiniteBoost;
        bool BoostEarningEnabled;
        bool BoostMessagesEnabled;
    };
}