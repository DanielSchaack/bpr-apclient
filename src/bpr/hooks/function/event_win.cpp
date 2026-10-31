#include "bpr/core/events.hpp"
#include "bpr/core/vehicles.hpp"
#include "bpr/hooks/game_hooks.hpp"
#include "detours.hpp"
#include <format>
#include <windows.h>
#include <intrin.h>
#include "../../app/app.hpp"

namespace EventWinLog
{

    // Wrapper in front of the event finisher (0x06E777F0). Cars are forwarded to the
    // finisher, other events (bikes) only get their profile flags set here, so this
    // one hook sees every win.
    constexpr std::uintptr_t Address = 0x07023BE0;
    using FinishFn = void (__thiscall*)(void*, void*, std::uint32_t, void*, std::int32_t);
    static FinishFn Original = nullptr;

    void OnWin(std::uint32_t eventId, bool isBike)
    {
        uint64_t vehicle_id = DetectActiveCar::GetActiveVehicleId();
        App::Instance->State().GetSaveData().AddEventWin(eventId, !isBike);

        if (App::Instance->State().GetSlotData().uniqueCarWins && !App::Instance->State().GetSaveData().HasCarWon(vehicle_id))
        {
            App::Instance->State().GetSaveData().AddCarWin(vehicle_id);

            const Vehicle::VehicleInfo* info = Vehicle::FindVehicleByID(vehicle_id);
            if (info && App::Instance->State().GetSlotData().uniqueCarWins == 1)
            {
                App::Instance->State().SendLocation(200 + info->unique_car_win_location_id);
            }

            if (info && App::Instance->State().GetSlotData().uniqueCarWins == 2)
            {
                int wins = App::Instance->State().GetSaveData().GetCarWins(info->boost_type);
                int offset = Vehicle::BoostTypeToGroupedOffset(info->boost_type);
                App::Instance->State().SendLocation(offset + wins - 1);
            }
        }

        if (App::Instance->State().GetSlotData().uniqueEventWins == 0)
        {
            if (isBike)
                App::Instance->State().SendLocation(eventId * 10);
            else
                App::Instance->State().SendLocation(eventId);
        }
        else
        {
            const Event::EventInfo* info = isBike
                                            ? Event::FindByBikeEventId(eventId)
                                            : Event::FindByEventId(eventId);
            if (info)
            {
                int wins = App::Instance->State().GetSaveData().GetEventWins(info->type);
                int offset = Event::EventTypeToGroupedOffset(info->type);
                App::Instance->State().SendLocation(offset + wins - 1);
            }
            else
            {
                Logger::Log(std::format("EventWinLog: no EventInfo for eventId {}", eventId));
            }
        }

        App::Instance->State().CheckGoal();

        if(App::Instance->State().GetSlotData().finishedEventsStayFinished)
            EnableEvent::MarkEventCompleted(eventId);

        GameHooks::PrintCurrentCarInformation();
    }

    // The wrapper forwards to the finisher when manager+0x2C404 is null or the
    // high nibble of the byte at +0xFC is not 1. Otherwise it takes the simple path.
    static bool TakesSimplePath(void* manager)
    {
        const auto* p = *reinterpret_cast<std::uint8_t* const*>(
            static_cast<std::uint8_t*>(manager) + 0x2C404);
        return p && (p[0xFC] >> 4) == 1;
    }

    // ECX = manager. EDX is unused by the original __thiscall function.
    // The remaining four arguments stay on the stack, exactly as in the game.
    void __fastcall Detour(void* manager, void*, void* actionQueue,
                          std::uint32_t eventId, void* results, std::int32_t mode)
    {
        const auto position = *reinterpret_cast<const std::int32_t*>(
            static_cast<const unsigned char*>(results) + 0x04);
        const bool isBike = TakesSimplePath(manager);

        Logger::Log(std::format("EventWinLog: raw eventId={} (low24={}) pos={} mode={} simple={}",
                            eventId, eventId & 0xFFFFFF, position, mode, isBike));

        Original(manager, actionQueue, eventId, results, mode);
        if (position == 1)
            OnWin(eventId, isBike);
    }

    constexpr std::uintptr_t kRankWins = 0x6A7B80 + 0x22C;      // int32[10], indexed by game mode
    constexpr int kRanks = 6, kMaxLevel = 6;
    using Ladder = std::array<int, kRanks>;

    int GameModeOf(int type)
    {
        switch (type)
        {
        case 1:  return 0;   // race
        case 2:  return 7;   // stunt run
        case 3:  return 3;   // road rage
        case 4:  return 8;   // marked man
        default: return -1;
        }
    }

    Ladder LadderOf(int type)
    {
        switch (type)
        {
        case 1:  return {0, 8, 18, 32, 50, 70};   // race
        case 2:  return {0, 1,  6, 10, 20, 25};   // stunt run
        case 3:  return {0, 4, 12, 20, 28, 33};   // road rage
        case 4:  return {0, 3,  6, 11, 17, 35};   // marked man
        default: return {};
        }
    }

    int WinsForLevel(const Ladder& l, int level)
    {
        const float pos = std::clamp(level, 0, kMaxLevel) * (kRanks - 1) / static_cast<float>(kMaxLevel);
        const int k = std::min(static_cast<int>(pos), kRanks - 2);
        return l[k] + static_cast<int>(std::lround((pos - k) * (l[k + 1] - l[k])));
    }

    void ApplyBaseDifficulty(const std::map<int, int>& base)
    {
        const std::uintptr_t game = GameHooks::GetGameModule();
        if (!game) return;
        auto* rankWins = reinterpret_cast<std::int32_t*>(game + kRankWins);

        for (const auto& [type, level] : base)
        {
            const int mode = GameModeOf(type);
            if (mode < 0 || level <= 0) continue;

            const int wins = WinsForLevel(LadderOf(type), level);
            if (rankWins[mode] < wins)
            {
                Logger::Log(std::format("Difficulty: type {} level {} -> rank wins {} -> {}", type, level, rankWins[mode], wins));
                rankWins[mode] = wins;
            }
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
