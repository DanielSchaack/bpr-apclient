#include "bpr/core/logger.hpp"
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

    using RebuildFn = void(__fastcall*)(void*, void*, const std::uint32_t*, int, int);
    static RebuildFn Original = nullptr;

    static std::mutex StateMutex;
    static std::uint64_t PrevRoad[3][2]{};

    static std::uint64_t Read64(void* self, std::uintptr_t off)
    {
        const auto* p = reinterpret_cast<const volatile std::uint32_t*>(
            static_cast<const std::uint8_t*>(self) + off);
        return (std::uint64_t(p[1]) << 32) | p[0];
    }

    // Is called on saves/loads and rebuilds the buffers for set road rules, using diffs to check which road has been ruled by the player
    static void __fastcall RebuildHook(void* self, void*, const std::uint32_t* src, int row, int col)
    {
        Original(self, nullptr, src, row, col);
        Logger::Log(std::format("Rebuilt road rule entries - row={}, col={}, src={}", row, col, *src));

        const std::uintptr_t slot = (std::uintptr_t(row) * 2 + col) * 8;
        const std::uint64_t cur0 = Read64(self, PrimaryBase + slot);

        std::lock_guard lock(StateMutex);

        if (row <= 2)
        {
            const std::uint64_t fresh = cur0 & ~PrevRoad[row][col];
            PrevRoad[row][col] = cur0;
            for (int bit = 0; bit < 64; ++bit)
                if (fresh & (1ull << bit))
                {
                    Logger::Log(std::format("Road Ruled - World={} - Type {} ({}) - ID={}", row, col, col ? "Showtime" : "Time", bit));
                    App::Instance->Gui().info_window->AddLogMessage(std::format("Road Ruled - World={} - Type {} ({}) - ID={}", row, col, col ? "Showtime" : "Time", bit));
                   // TODO: App::Instance->State().SendLocation(RoadLocationId(row, bit, col == 1));
                }
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
