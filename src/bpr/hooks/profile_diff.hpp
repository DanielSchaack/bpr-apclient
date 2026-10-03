#pragma once
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <format>
#include <thread>
#include <vector>
#include "bpr/core/logger.hpp"

class ProfileDiff
{
public:
    static void Start()
    {
        if (s_running.exchange(true)) return;
        s_thread = std::thread(&ProfileDiff::Loop);
        s_thread.detach();
    }

    static void Stop() { s_running = false; }

private:
    static constexpr std::uintptr_t kGamePtr   = 0x013FC8E0;
    static constexpr std::size_t    kStateOff  = 0xB6D464;
    // static constexpr std::size_t    kProfileOff = 0x8681D0;
    // static constexpr std::size_t kSize       = 0x65DA0;
    static constexpr std::size_t kProfileOff = 0x6A7B80; //runtime
    static constexpr std::size_t kSize       = 0x292D0;

    struct Field { std::uint32_t off; std::uint32_t size; const char* name; };
        static constexpr Field kFields[] = {
        {0x64,    4,      "distOnline"},
        {0x68,    4,      "distOffline"},
        {0x6C,    4,      "inCarTime"},
        {0x70,    4,      "rank+powerParking"},
        {0x74,    4,      "bestBurnoutChain"},
        {0x78,    0x44,   "modeAmount[17]"},
        {0xBC,    0x44,   "modeDiscovered[17]"},
        {0x100,   0x44,   "modeCompleted[17]"},
        {0x144,   0x44,   "modeCompletedSinceStart[17]"},
        {0x188,   4,      "totalTakedowns"},
        {0x18C,   4,      "verticalTakedowns"},
        {0x190,   0x34,   "takedownTypes[13]"},
        {0x1C4,   0x28,   "winsPerMode[10]"},
        {0x1EC,   0x28,   "rankWins[10]"},
        {0x214,   0x28,   "losses[10]"},
        {0x23C,   4,      "barrelRolls"},
        {0x240,   0x14,   "recordFloats(spin,hbrake,drift,oncoming,air)"},
        {0x254,   4,      "showtimeRecord"},
        {0x258,   4,      "stuntRunScore"},
        {0x25C,   0x10,   "counts(car,livery,rival,event)"},
        {0x270,   0x3000, "cars[512]"},
        {0x3270,  0x3000, "liveries[512]"},
        {0x6270,  0xE00,  "rivals[64]"},
        {0x7070,  0x578,  "events[175]"},
        {0x75E8,  0x3018, "stuntElements[3]"},
        {0xA600,  4,      "medalCount"},
        {0xA604,  4,      "goldSilverFlags"},
        {0xA608,  0x30,   "junkyardSet"},
        {0xA638,  0x60,   "bodyShopSet"},
        {0xA698,  0x30,   "paintShopSet"},
        {0xA6C8,  0x78,   "gasStationSet"},
        {0xA740,  0x60,   "carParkSet"},
        {0xA7A0,  0x3E88, "freeburn"},
        {0xE628,  0x9280, "hitProps"},
        {0x178A8, 0x1E,   "stuntCounts"},
        {0x1BF50, 4,      "lastRoadRulesReset"},
        {0x18C20, 0x1400, "networkChallenges[64]"},
        {0x1A020, 0x0A00, "playerChallenges[64]"},
        {0x1AA20, 0x0200, "mode1Cars[64]"},
        {0x1AC20, 0x0200, "mode2Cars[64]"},
        {0x1AE20, 0x0100, "mode1Scores[64]"},
        {0x1AF20, 0x0100, "mode2Scores[64]"},
        {0x1BEA0, 0x0040, "mode1Flags[64]"},
        {0x1BEE0, 0x0040, "mode2Flags[64]"},
        {0x1BF20, 4,      "unknownAfterFlags"},
        {0x1BF50, 4,      "lastRoadRulesReset"},
        {0x28B78, 0x01E0, "extraChallenges[12]"},
        {0x28D58, 0x0060, "extraMode1Cars[12]"},
        {0x28DB8, 0x0060, "extraMode2Cars[12]"},
        {0x28E18, 0x0030, "extraMode1Scores[12]"},
        {0x28E48, 0x0030, "extraMode2Scores[12]"},
        {0x29130, 0x000C, "extraMode1Flags"},
        {0x2913C, 0x000C, "extraMode2Flags"},
        {0x65D18, 0x20,   "seenTraining"},
        {0x65D38, 0x0C,   "onlineRaces/won/mugshotsSent"},
        {0x65D5C, 4,      "roadRageRecord"},
        {0x65D60, 8,      "seenTrophy"},
        {0x65D68, 8,      "achievements"},
        {0x65D70, 8,      "completionFlags"},
        {0x65D90, 4,      "realTimePlayed"},
    };
    inline static std::atomic<bool> s_knownOnly{true};

    // returns nullptr if offset is outside every known field
    static const char* Describe(std::uint32_t off, std::uint32_t& rel)
    {
        for (const auto& f : kFields)
            if (off >= f.off && off < f.off + f.size) { rel = off - f.off; return f.name; }
        return nullptr;
    }

    inline static std::atomic<bool> s_running{false};
    inline static std::thread s_thread;
    inline static std::vector<std::uint8_t> s_snap;

    static std::uint8_t* Profile()
    {
        auto* game = *reinterpret_cast<std::uint8_t**>(kGamePtr);
        if (!game) return nullptr;
        if (*reinterpret_cast<std::uint32_t*>(game + kStateOff) != 1) return nullptr;
        return game + kProfileOff;
    }

    static bool SafeCopy(void* dst, const void* src, std::size_t n)
    {
        SIZE_T got = 0;
        return ReadProcessMemory(GetCurrentProcess(), src, dst, n, &got) && got == n;
    }

    static bool GameIsForeground()
    {
        DWORD pid = 0;
        GetWindowThreadProcessId(GetForegroundWindow(), &pid);
        return pid == GetCurrentProcessId();
    }

    static void Snapshot()
    {
        auto* p = Profile();
        if (!p) { Logger::Log("ProfileDiff: game not ready"); return; }
        std::vector<std::uint8_t> tmp(kSize);
        if (!SafeCopy(tmp.data(), p, kSize)) { Logger::Log("ProfileDiff: read failed"); return; }
        s_snap = std::move(tmp);
        Logger::Log("ProfileDiff: snapshot taken (F8)");
    }

    static void Diff()
    {
        auto* p = Profile();
        if (!p || s_snap.empty()) { Logger::Log("ProfileDiff: no snapshot / game not ready"); return; }

        std::vector<std::uint8_t> now(kSize);
        if (!SafeCopy(now.data(), p, kSize)) { Logger::Log("ProfileDiff: read failed"); return; }

        int changes = 0;
        Logger::Log("ProfileDiff: ---- diff (F9) ----");
        for (std::size_t i = 0; i + 4 <= kSize; i += 4)
        {
            std::uint32_t a, b;
            std::memcpy(&a, &s_snap[i], 4);
            std::memcpy(&b, &now[i], 4);
            if (a == b) continue;

            std::uint32_t rel = 0;
            const char* name = Describe(static_cast<std::uint32_t>(i), rel);
            if (!name && s_knownOnly) continue;          // skip noise
            if (!name) {
                // only small, count-like changes: 0 <= old,new < 0x10000, delta 1..16
                if (a >= 0x10000 || b >= 0x10000 || b <= a || b - a > 16) continue;
            }
            Logger::Log(std::format("  profile+{:#07x} ({}+{:#x}): {:#010x} -> {:#010x}  [CE: [0x013FC8E0]+{:X}]",
                                    i, name ? name : "UNMAPPED", rel, a, b, kProfileOff + i));
        }
        Logger::Log(std::format("ProfileDiff: {} changed dwords", changes));
        s_snap = std::move(now);   // re-arm for the next action

    }

    static void Loop()
    {
        bool f7Prev = false, f8Prev = false, f9Prev = false;
        while (s_running)
        {
            bool fg = GameIsForeground();
            bool f7 = fg && (GetAsyncKeyState(VK_F7) & 0x8000);
            if (f7 && !f7Prev) {
                s_knownOnly = !s_knownOnly;
                Logger::Log(std::format("ProfileDiff: knownOnly = {}", s_knownOnly.load()));
            }
            f7Prev = f7;
            bool f8 = fg && (GetAsyncKeyState(VK_F8) & 0x8000);
            bool f9 = fg && (GetAsyncKeyState(VK_F9) & 0x8000);

            if (f8 && !f8Prev) Snapshot();
            if (f9 && !f9Prev) Diff();

            f8Prev = f8; f9Prev = f9;
            Sleep(20);
        }
    }
};
