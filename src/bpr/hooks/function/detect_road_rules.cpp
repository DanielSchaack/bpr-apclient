#include "bpr/core/logger.hpp"
#include "bpr/core/map.hpp"
#include "detours.hpp"
#include "MinHook.h"
#include <format>
#include <windows.h>
#include <intrin.h>
#include "../../app/app.hpp"

namespace DetectRoadRules
{
    constexpr std::uintptr_t RebuildAddress = 0x070296B0;
    constexpr std::uintptr_t PrimaryBase    = 0x37E4 * 8;   // masks for bits 0..63
    constexpr std::uintptr_t IslandEntries  = 0x28B78;      // ChallengePlayerScoreEntry[12], 0x28 bytes each
    constexpr std::uintptr_t IslandEntrySize = 0x28;
    constexpr int            IslandRoads    = 12;

    using RebuildFn = void(__fastcall*)(void*, void*, const std::uint32_t*, int, int);
    static RebuildFn Original = nullptr;

    static std::mutex StateMutex;
    static std::uint64_t PrevRoad[3][2]{};
    static bool PrevIsland[IslandRoads][2]{};

    static std::uint64_t Read64(void* self, std::uintptr_t off)
    {
        const auto* p = reinterpret_cast<const volatile std::uint32_t*>(
            static_cast<const std::uint8_t*>(self) + off);
        return (std::uint64_t(p[1]) << 32) | p[0];
    }

    static void ReportRoadRuled(const char* world, int row, int col, int id)
    {
        const std::string msg = std::format("Road Ruled - {} {} - Type {} ({}) - ID={}", world, row, col, col ? "Showtime" : "Time", id);
        Logger::Log(msg);
        App::Instance->Gui().info_window->AddLogMessage(msg);

        if(const Map::RoadInfo* info = Map::FindRoadInfoByID(id))
        {
            const std::string_view w{world};
            if (!row)
            {
                App::Instance->State().CacheBreakable(static_cast<int>(info->area), 4 + col);
            }
            else if (row)
            {
                App::Instance->State().CacheBreakable(static_cast<int>(info->area), 6 + col);
            }
        }
    }

    // Is called on saves/loads and rebuilds the buffers for set road rules, using diffs to check which road has been ruled by the player
    static void __fastcall RebuildHook(void* self, void*, const std::uint32_t* src, int row, int col)
    {
        Original(self, nullptr, src, row, col);
        Logger::Log(std::format("Rebuilt road rule entries - row={}, col={}, src={}", row, col, *src));

        std::lock_guard lock(StateMutex);

        if (row <= 2)
        {
            const std::uintptr_t slot = (std::uintptr_t(row) * 2 + col) * 8;
            const std::uint64_t cur0 = Read64(self, PrimaryBase + slot);

            const std::uint64_t fresh = cur0 & ~PrevRoad[row][col];
            PrevRoad[row][col] = cur0;
            for (int bit = 0; bit < 64; ++bit)
                if (fresh & (1ull << bit))
                    ReportRoadRuled("Car", row, col, bit);
            return;
        }
    }

    MH_STATUS Install()
    {
        return MH_CreateHook(reinterpret_cast<void*>(RebuildAddress),
                             reinterpret_cast<void*>(&RebuildHook),
                             reinterpret_cast<void**>(&Original));
    }
}
