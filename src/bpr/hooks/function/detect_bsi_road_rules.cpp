
#include "MinHook.h"
#include "bpr/core/logger.hpp"
#include "bpr/app/app.hpp"
#include <cstdint>
#include <format>
namespace DetectIslandRoadRules
{
    constexpr std::uintptr_t Address = 0x07029EF0;

    constexpr std::size_t kSrcIsland        = 0xA00;        // island entries follow the 64 mainland ones in src
    constexpr std::size_t kIslandCarEntries = 0x28B78;
    constexpr std::size_t kIslandScores[3]  = {0, 0x28E18, 0x28E48};   // modes 1, 2: derived time scores
    constexpr int         kSlots            = 12;
    constexpr std::uint32_t kNoScore        = 0xFFFFFFFF;

    struct Entry                                            // ChallengePlayerScoreEntry, 0x28 bytes
    {
        std::uint64_t dirty;                                // +0x00
        std::uint64_t valid;                                // +0x08  bit 0 = time record, bit 1 = showtime record
        std::int32_t  score[2];                             // +0x10 time (ms), +0x14 showtime
        std::uint64_t car[2];                               // +0x18, +0x20
    };
    static_assert(sizeof(Entry) == 0x28);

    using SyncFn = void (__thiscall*)(void* profile, const std::uint8_t* src, std::int32_t mode);
    static SyncFn Original = nullptr;

    static void Report(int mode, int slot, int type, std::int32_t score, std::uint64_t car)
    {
        const uint64_t id = 64 + slot;
        const int ruledType = mode ? mode + 1 : type;   // 0 time, 1 showtime, 2 bike day, 3 bike night
        if (!App::Instance->State().MarkRoadRuled(ruledType, id))
            return;   // already reported
 
        static constexpr const char* kTable[] = {"car", "bike day", "bike night"};
        const std::string msg = std::format("Island road rule record: {} slot {}{} - {} ms",
                                            kTable[mode], slot,
                                            mode == 0 ? (type ? " (showtime)" : " (time)") : "", score);
        Logger::Log(std::format("{} | car {}", msg, car));
        App::Instance->Gui().info_window->AddLogMessage(msg);
        if(const Map::RoadInfo* info = Map::FindRoadInfoByID(id))
        {
            if (!mode)
            {
                App::Instance->State().CacheBreakable(static_cast<int>(info->area), 4 + type);
            }
            else if (mode)
            {
                App::Instance->State().CacheBreakable(static_cast<int>(info->area), 5 + mode);
            }
        }
    }

    static void Detect(const std::uint8_t* profile, const std::uint8_t* src, int mode)
    {
        const auto* in = reinterpret_cast<const Entry*>(src + kSrcIsland);

        if (mode == 0)                                      // car: raw entries, time and showtime
        {
            const auto* cur = reinterpret_cast<const Entry*>(profile + kIslandCarEntries);
            for (int slot = 0; slot < kSlots; ++slot)
                for (int type = 0; type < 2; ++type)
                {
                    if (!((in[slot].valid >> type) & 1)) continue;
                    const bool had = (cur[slot].valid >> type) & 1;
                    if (!had || in[slot].score[type] != cur[slot].score[type])
                        Report(0, slot, type, in[slot].score[type], in[slot].car[type]);
                }
            return;
        }

        const auto* scores = reinterpret_cast<const std::uint32_t*>(profile + kIslandScores[mode]);
        for (int slot = 0; slot < kSlots; ++slot)           // bike day / night: one score per road
        {
            if (!(in[slot].valid & 1)) continue;
            if (scores[slot] == kNoScore || scores[slot] != static_cast<std::uint32_t>(in[slot].score[0]))
                Report(mode, slot, 0, in[slot].score[0], in[slot].car[0]);
        }
    }

    static void __fastcall Detour(void* profile, void*, const std::uint8_t* src, std::int32_t mode)
    {
        // log when the source pointer for a mode changes (and on the first call), not on every call
        static std::uintptr_t lastSrc[3] = {};
        if (mode >= 0 && mode <= 2 && lastSrc[mode] != reinterpret_cast<std::uintptr_t>(src))
        {
            lastSrc[mode] = reinterpret_cast<std::uintptr_t>(src);
            Logger::Log(std::format("RoadRules sync: mode {} src {:#010x}", mode, lastSrc[mode]));
        }

        if (src && mode >= 0 && mode <= 2)
            Detect(static_cast<const std::uint8_t*>(profile), src, mode);   // before Original: afterwards profile == src

        Original(profile, src, mode);
    }

    MH_STATUS Install()
    {
        return MH_CreateHook(reinterpret_cast<void*>(Address),
                             reinterpret_cast<void*>(&Detour),
                             reinterpret_cast<void**>(&Original));
    }
}
