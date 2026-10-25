#include "bpr/core/logger.hpp"
#include "bpr/core/mod_config.hpp"
#include "bpr/core/map.hpp"
#include "bpr/core/vehicles.hpp"
#include "bpr/hooks/structure/detours.hpp"
#include "bpr/hooks/function/detours.hpp"
#include "bpr/hooks/game_hooks.hpp"
#include "bpr/app/app.hpp"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <format>
#include <vector>

namespace Filler
{
    // Game memory, relative to the game module (*0x013FC8E0)
    constexpr std::uintptr_t kTrafficDensity  = 0x6E4764;   // float, 0 to 5
    constexpr std::uintptr_t kTrafficOverride = 0x6E4768;   // bool
    constexpr std::uintptr_t kBoostTypeOffset = 0x3FFD4;    // int32, the other mod reads it as (value - 1)
    constexpr std::uintptr_t kBoostLevel      = 0x40754;
    constexpr std::uintptr_t kBoostLossLevel  = 0x40758;
    constexpr std::uintptr_t kGameState       = 0x69B000;

    // Teleport target (your F6 position)
    constexpr float kTeleportX = 3024.2f, kTeleportY = -3.8f, kTeleportZ = -1990.4f;

    // Traffic state, restored by Tick()
    bool  trafficActive = false;
    float oldDensity = 0.0f;
    bool  oldOverride = false;
    std::chrono::steady_clock::time_point trafficEnd;

    // Swaps held back while an event runs
    std::uint64_t g_pendingCar = 0;
    int randomCarSwaps = 0;

    // Random integer in [0, max). std::rand is not perfect, but it is plenty for a joke effect.
    int RandomBelow(int max)
    {
        static bool seeded = false;
        if (!seeded) { std::srand(static_cast<unsigned>(std::time(nullptr))); seeded = true; }
        return std::rand() % max;
    }

    float RandomFloat(float min, float max)
    {
        return min + (max - min) * (std::rand() / static_cast<float>(RAND_MAX));
    }

    // 100: Refill current boost meter
    void BoostRefill(void* gameActionQueue)
    {
        Logger::Log(std::format("Received Filler 'Boost Refill'"));
        GameActions::GameAction_SetBoost set_boost{};
        set_boost.Flags.BoostAmount = true;
        set_boost.BoostAmount = 1.0f;
        set_boost.ActiveRaceVehicleIndex = GameHooks::GetPlayerCarIndex();
        GameActions::AddGameAction(gameActionQueue, &set_boost, set_boost.ID, sizeof(set_boost));
    }

    // 101: random boost type, never the current one
    void RandomBoostType(void* gameActionQueue)
    {
        Logger::Log(std::format("Received Filler 'Boost Swap'"));
        const std::uintptr_t game = GameHooks::GetGameModule();
        const std::uintptr_t vehicleData = GameHooks::GetRaceVehicleAddress();   // see the note about vehicleData below
        if (!game || !vehicleData) return;

        const int current = *reinterpret_cast<std::int32_t*>(game + kBoostTypeOffset) - 1;

        // Speed = 0, Aggression = 1, Stunt = 2; pick one that is not the current type
        int next = RandomBelow(2);     // 0 or 1
        if (next >= current) next++;        // skips the current value: result is always 0..2 and != current
        if (next > 2) next = 2;             // safety if current was out of range

        GameActions::GameAction_UpdateVehicleStats action{};
        action.Speed          = *reinterpret_cast<std::uint8_t*>(vehicleData + 0x99);
        action.Strength       = *reinterpret_cast<std::uint8_t*>(vehicleData + 0x9B);
        action.BoostLossLevel = *reinterpret_cast<std::int32_t*>(game + kBoostLossLevel);
        action.BoostLevel     = *reinterpret_cast<std::int32_t*>(game + kBoostLevel);
        action.DamageLimit    = *reinterpret_cast<float*>(vehicleData + 0x90);
        action.BoostType      = static_cast<GameActions::BoostType>(next);

        GameActions::AddGameAction(gameActionQueue, &action, action.ID, sizeof(action));
        Logger::Log(std::format("Boost type {} -> {}", current, next));
    }

    // In an event when the in-event flag is set and the event handle is valid (checked against the F6 log)
    bool EventActive()
    {
        const std::uintptr_t game = GameHooks::GetGameModule();
        return game && *reinterpret_cast<const std::uint32_t*>(game + 0x69B000 + 0x2598) != 0;
    }

    bool SwapBlockedByEvent() { return !ModConfig::Get().allowSwapInEvents && EventActive(); }

    void SwapToCar(void* gameEventQueue, std::uint64_t vehicleId)
    {
        // Only swap within car type
        if (DetectActiveCar::IsBikeActive() != Vehicle::IsBike(vehicleId))
            return;

        if (SwapBlockedByEvent())
        {
            g_pendingCar = vehicleId;
            Logger::Log(std::format("Car swap to '{:x}' waits for the event to end", vehicleId));
            return;
        }

        GameActions::GameEvent_ChangePlayerVehicle event{};
        event.VehicleID = vehicleId;
        event.WheelID = 0;
        event.ResetPlayerCamera = true;
        event.KeepResetSection = true;

        GameActions::GameEventQueue_AddGameEvent(gameEventQueue, &event, event.ID,
                                                sizeof(event));
        Logger::Log(std::format("Changed car to '{:x}'", vehicleId));
        g_fixFlagPending = true;                            // MinimapFix repairs the player flags
    }

    // 102: random obtained car that is not the current one
    void SwapToRandomCar(void* gameEventQueue)
    {
        if (SwapBlockedByEvent())
        {
            randomCarSwaps++;
            Logger::Log("Random car swap waits for the event to end");
            return;
        }

        Logger::Log(std::format("Received Filler 'Car Swap'"));
        const auto& owned = App::Instance->State().GetSaveData().obtained_cars;
        const Vehicle::VehicleInfo* current = DetectActiveCar::GetActiveVehicle();

        Logger::Log(std::format("Random car: obtained_cars={}, current='{}' (slot id {})",
                            owned.size(),
                            current ? current->proper_name : "unknown",
                            DetectActiveCar::GetActiveVehicleId()));

        std::vector<const Vehicle::VehicleInfo*> others;
        for (std::uint64_t carId : owned)
        {
            const Vehicle::VehicleInfo* info = Vehicle::FindVehicleByID(carId);
            if (info && info != current)
                others.push_back(info);
        }
        if (others.empty())
        {
            Logger::Log("Random car: no other car obtained");
            return;
        }

        const Vehicle::VehicleInfo* chosen = others[RandomBelow(static_cast<int>(others.size()))];
        for (int attempt = 0; attempt < 5; ++attempt) {
            chosen = others[RandomBelow(static_cast<int>(others.size()))];
            if (DetectActiveCar::IsBikeActive() == Vehicle::IsBike(chosen->id))
                break;
        }

        SwapToCar(gameEventQueue, chosen->id);
    }

    // 103: high traffic density for 60 seconds
    void HighTraffic()
    {
        Logger::Log(std::format("Received Filler 'High Traffic Density'"));
        const std::uintptr_t game = GameHooks::GetGameModule();
        if (!game) return;

        auto* density  = reinterpret_cast<float*>(game + kTrafficDensity);
        auto* override = reinterpret_cast<bool*>(game + kTrafficOverride);

        if (!trafficActive)                 // keep the real values, even if the item arrives twice
        {
            oldDensity = *density;
            oldOverride = *override;
            trafficActive = true;
        }

        *density = 5.0f;
        *override = true;
        trafficEnd = std::chrono::steady_clock::now() + std::chrono::seconds(60);
        Logger::Log(std::format("Traffic density {:.2f} for 60 s", *density));
    }

    bool should_teleport = false;
    float teleport_position[4] = {0.0f, 0.0f, 0.0f, 0.0f };
    void QueueTeleport(float Position[4])
    {
        should_teleport = true;
        std::copy(Position, Position + 4, teleport_position);
    }

    void TeleportToPosition(void* gameEventQueue, const float Position[4])
    {
        GameActions::GameEvent_TeleportPlayerVehicle event{};
        event.Position[0] = Position[0];
        event.Position[1] = Position[1];
        event.Position[2] = Position[2];
        event.Position[3] = Position[3];
        event.Direction[0] = 0.0f;
        event.Direction[1] = 0.0f;
        event.Direction[2] = 1.0f;
        event.Direction[3] = 0.0f;

        GameActions::GameEventQueue_AddGameEvent(gameEventQueue, &event, event.ID, sizeof(event));
        Logger::Log("Teleported");
        g_fixFlagPending = true;
    }

    // 104: teleport to the fixed spot
    void TeleportToRandomDriveThru(void* gameEventQueue)
    {
        Logger::Log(std::format("Received Filler 'Random Teleport'"));

        // constexpr size_t driveThruInfosCount = std::size(Map::driveThruInfos); // TODO: Map out all drive thrus
        const Map::DriveThruInfo chosen = Map::driveThruInfos[RandomBelow(6)];
        TeleportToPosition(gameEventQueue, chosen.Position);
    }

    bool Handle(std::int64_t itemId, void* gameActionQueue, void* gameEventQueue)
    {
        switch (itemId)
        {
        case 100: BoostRefill(gameActionQueue);             return true;
        case 101: RandomBoostType(gameActionQueue);         return true;
        case 102: SwapToRandomCar(gameEventQueue);          return true;
        case 103: HighTraffic();                            return true;
        case 104: TeleportToRandomDriveThru(gameEventQueue); return true;
        default:  return false;
        }
    }

    void Tick(void* gameActionQueue, void* gameEventQueue)
    {
        if (trafficActive && std::chrono::steady_clock::now() >= trafficEnd)
        {
            if (const std::uintptr_t game = GameHooks::GetGameModule())
            {
                *reinterpret_cast<float*>(game + kTrafficDensity) = oldDensity;
                *reinterpret_cast<bool*>(game + kTrafficOverride) = oldOverride;
            }
            trafficActive = false;
            Logger::Log("Traffic density restored");
        }

        if(should_teleport)
        {
            should_teleport = false;
            TeleportToPosition(gameEventQueue, static_cast<const float*>(teleport_position));
        }

        // swaps that waited for an event to end
        if (SwapBlockedByEvent())
            return;
        if (g_pendingCar)
        {
            const std::uint64_t id = g_pendingCar;
            g_pendingCar = 0;
            SwapToCar(gameEventQueue, id);
        }
        else if (randomCarSwaps > 0)
        {
            randomCarSwaps--;
            SwapToRandomCar(gameEventQueue);
        }
    }

    bool Install()
    {
        return true;
    }
}


// After a car swap inside an event the game leaves RaceCar+0xAA/+0xAB of the player at 0. The GUI entity
// registration skips entities with +0xAA == 0 while the filter byte is set, so the player's minimap and
// compass entry disappears. This puts the two bytes back.
namespace MinimapFix
{
    constexpr std::uintptr_t Address = 0x0092B0F0;        // FUN_0092b0f0(gui, msg, id), assumed ret 8
    constexpr int kTimeoutMessages = 600;                 // give up after about 10 s of 0xE4 messages

    using Fn = std::uintptr_t (__thiscall*)(void* gui, const std::uint8_t* msg, std::int32_t id);
    static Fn Original = nullptr;

    static void Repair()
    {
        static int waited = 0;
        if (++waited > kTimeoutMessages) { Filler::g_fixFlagPending = false; waited = 0; return; }

        auto* game = *reinterpret_cast<std::uint8_t**>(0x013FC8E0);
        if (!game || game[0x40D84] == 0) return;            // entity filter off: nothing to repair

        const auto idx = *reinterpret_cast<const std::int32_t*>(game + 0x40C28);
        if (idx < 0 || idx > 7) return;
        auto* me = *reinterpret_cast<std::uint8_t**>(game + 0x12980 + idx * 0x4180 + 0x7C0);   // the player's RaceCar
        if (!me || me[0xA8] == 3) return;

        if (me[0xAA] == 0)                                  // the game has reset it: put it back
        {
            me[0xAA] = game[0x40D84];
            me[0xAB] = game[0x40D87];
            Filler::g_fixFlagPending = false;
            waited = 0;
        }
    }

    static std::uintptr_t __fastcall Detour(void* gui, void*, const std::uint8_t* msg, std::int32_t id)
    {
        if (id == 0xE4 && Filler::g_fixFlagPending)
            Repair();
        return Original(gui, msg, id);
    }

    MH_STATUS Install()
    {
        return MH_CreateHook(reinterpret_cast<void*>(Address),
                             reinterpret_cast<void*>(&Detour),
                             reinterpret_cast<void**>(&Original));
    }
}
