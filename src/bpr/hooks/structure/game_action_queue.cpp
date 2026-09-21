 #include "detours.hpp"
namespace GameActions{
    bool AddGameAction(void* gameActionQueue, const void* gameAction, int32_t gameActionID, int32_t gameActionSize)
    {
        bool result = false;

        __asm
        {
            push dword ptr [gameActionSize]
            push dword ptr [gameActionID]
            push dword ptr [gameAction]
            mov ecx, dword ptr [gameActionQueue]

            mov eax, 0x004C0590
            call eax

            mov byte ptr [result], al
        }

        return result;
    }

    
    // void D
    // BPR::GameAction_UpdateVehicleStats gameAction =
    //     {
    //         .Speed          = vehicleData.at(0x99).as<uint8_t>(),
    //         .Strength       = vehicleData.at(0x9B).as<uint8_t>(),
    //         .BoostLossLevel = gameModule.at(0x40758).as<int32_t>(),
    //         .BoostLevel     = gameModule.at(0x40754).as<int32_t>(),
    //         .DamageLimit    = vehicleData.at(0x90).as<float>(),
    //         .BoostType      = static_cast<BPR::BoostType>(gameModule.at(0x3FFD4).as<int32_t>() - 1),
    //     };
    //     BPR::GameActionQueue_AddGameAction(gameActionQueue, &gameAction, gameAction.ID, sizeof(gameAction));
}