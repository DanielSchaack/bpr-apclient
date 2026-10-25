#pragma once
#include <cmath>
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <format>
#include <string>
#include <thread>
#include <vector>
#include "bpr/core/logger.hpp"
#include "bpr/app/app.hpp"
#include "bpr/hooks/function/detours.hpp"

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
        {0x1BF20, 0x30,   "roadRuleMasks[3][2]"},
        {0x282F8, 0xFD8,  "islandObject"},
        {0x2873C, 0x08,   "islandJunkyardFound(id,count)"},
        {0x28744, 0x08,   "islandAutoRepairFound(id,count)"},
        {0x287B8, 0x3C0,  "islandBestRoadRules[12]"},
        {0x28B78, 0x1E0,  "islandCarRoadRules[12]"},
        {0x28D58, 0x60,   "islandBikeDayCars[12]"},
        {0x28DB8, 0x60,   "islandBikeNightCars[12]"},
        {0x28E18, 0x30,   "islandBikeDayScores[12]"},
        {0x28E48, 0x30,   "islandBikeNightScores[12]"},
        {0x28E78, 0x60,   "islandBikeFriendsBest[12][2]"},
        {0x28ED8, 0x258,  "islandBikeFriendNames"},
        {0x29130, 0x0C,   "islandBikeDayDirty[12]"},
        {0x2913C, 0x0C,   "islandBikeNightDirty[12]"},
        {0x65D18, 0x20,   "seenTraining"},
        {0x65D38, 0x0C,   "onlineRaces/won/mugshotsSent"},
        {0x65D5C, 4,      "roadRageRecord"},
        {0x65D60, 8,      "seenTrophy"},
        {0x65D68, 8,      "achievements"},
        {0x65D70, 8,      "completionFlags"},
        {0x65D90, 4,      "realTimePlayed"},
    };
    inline static std::atomic<bool> s_knownOnly{true};

    static constexpr std::size_t kGameStateOff  = 0x69B000;
    static constexpr std::size_t kActiveSlotOff = kGameStateOff + 0x45480;   // active player slot, -1 = none
    static constexpr std::size_t kPlayerRecOff  = kGameStateOff + 0x42C20;   // record array, 0x530 bytes per slot
    static constexpr std::size_t kPlayerRecSize = 0x530;
    struct Position { float x, y, z; };
    inline static Position s_snapPos{};
    inline static bool     s_snapPosValid = false;

    template <class T>
    static bool Rd(std::uintptr_t addr, T& out)
    {
        return SafeCopy(&out, reinterpret_cast<const void*>(addr), sizeof(T));
    }

    // First 12 bytes of the player's record. Layout is a guess: three floats, x/y/z in some order.
    static bool GetPosition(Position& out)
    {
        auto* game = *reinterpret_cast<std::uint8_t**>(kGamePtr);
        if (!game) return false;

        std::int32_t slot = -1;
        if (!SafeCopy(&slot, game + kActiveSlotOff, sizeof(slot)) || slot < 0 || slot > 64)
            return false;

        float v[3];
        if (!SafeCopy(v, game + kPlayerRecOff + static_cast<std::size_t>(slot) * kPlayerRecSize, sizeof(v)))
            return false;
        for (float f : v)
            if (!std::isfinite(f) || std::fabs(f) > 100000.0f) return false;   // not a plausible coordinate

        out = {v[0], v[1], v[2]};
        return true;
    }

    static void LogPosition(const char* tag)
    {
        Position p;
        if (GetPosition(p))
        {
            Logger::Log(std::format("ProfileDiff: position ({}) = {{{:.1f}f, {:.1f}f, {:.1f}f, 1.0f}}", tag, p.x, p.y, p.z));
            App::Instance->Gui().info_window->AddLogMessage(std::format("ProfileDiff: position ({}) = {{{:.1f}f, {:.1f}f, {:.1f}f, 1.0f}}", tag, p.x, p.y, p.z));
        }
        else
            Logger::Log(std::format("ProfileDiff: position ({}) unavailable", tag));
    }

    // BrnGui::GuiPlayerInfo is a member of the GUI object at gui + 0x7A90; gui = game + 0x8E8430
    static constexpr std::size_t kGuiObject = 0x8E8430;

    // one line: what the GUI holds, what the producer sent, and the writer gate
    static void LogGuiPlayerInfo(const char* tag)
    {
        auto* game = *reinterpret_cast<std::uint8_t**>(kGamePtr);
        if (!game) return;

        std::uint32_t d[20] = {};                                        // GuiPlayerInfo, 0x50 bytes
        if (!SafeCopy(d, game + kGuiObject + 0x7A90, sizeof(d))) return;

        float x, y, z;
        std::memcpy(&x, &d[0], 4);  std::memcpy(&y, &d[1], 4);  std::memcpy(&z, &d[2], 4);
        const int speed = std::abs(static_cast<std::int32_t>(d[0x30 / 4]));

        std::int32_t playerIdx = -1;
        std::uint8_t flags[8] = {};
        SafeCopy(&playerIdx, game + kGuiObject + 0x7AB8, sizeof(playerIdx));
        SafeCopy(flags,      game + kGuiObject + 0xD234, sizeof(flags));
        const bool gateOk = playerIdx >= 0 && playerIdx < 8 && flags[playerIdx] == 1;

        std::string kinds;

        Logger::Log(std::format("[{}] gui pos {:.1f} {:.1f} {:.1f} | speed {} | player idx {} | flags {:02x}{:02x}{:02x}{:02x}{:02x}{:02x} | gate {} | kinds {} ",
                                tag, x, y, z, speed, playerIdx,
                                flags[0], flags[1], flags[2], flags[3], flags[4], flags[5],
                                gateOk ? "open" : "CLOSED", kinds.empty() ? "-" : kinds));
    }

    // one line: for each entity   index:role/flag/slot   (a * marks entities the registration loop skips)
    static void LogRaceCars(const char* tag)
    {
        auto* game = *reinterpret_cast<std::uint8_t**>(kGamePtr);
        if (!game) return;

        std::uint8_t filter = 0;
        SafeCopy(&filter, game + 0x40D84, 1);                        // param_1 + 0x30084

        std::string s = std::format("[{}] entity filter {} |", tag, filter);
        for (int i = 0; i < 8; ++i)
        {
            std::uint8_t e[0xC0];
            if (!SafeCopy(e, game + 0x10F40 + i * 0xC0, sizeof(e))) continue;
            const bool registered = e[0xA8] != 3 && (filter == 0 || e[0xAA] != 0);
            s += std::format(" {}:{}/{}/{}{}", i, e[0xA8], e[0xAA], static_cast<std::int8_t>(e[0xB0]), registered ? "" : "*");
        }
        Logger::Log(s);
    }

    // event data record for an event ID: gs+0x38E28 is the event table (count +0x1C, entries +0x18, 5 dwords each,
    // [0] = id, [1] = record for cars, [3] = record for bikes), picked by the vehicle class (gs+0x38E14 -> +0xFC >> 4)
    static bool FindEventData(std::uintptr_t gs, std::uint32_t id, std::uintptr_t& out)
    {
        std::uintptr_t vehicle = 0, tbl = 0;
        Rd(gs + 0x38E14, vehicle);
        Rd(gs + 0x38E28, tbl);
        if (!tbl) return false;

        std::uint32_t cls = 0xFFFFFFFFu;
        std::uint8_t b = 0;
        if (vehicle && Rd(vehicle + 0xFC, b)) cls = b >> 4;

        std::uint32_t count = 0;
        std::uintptr_t entries = 0;
        if (!Rd(tbl + 0x1C, count) || !Rd(tbl + 0x18, entries) || !entries || count > 4096) return false;

        for (std::uint32_t i = 0; i < count; ++i)
        {
            std::uint32_t e[5];
            if (!Rd(entries + i * 0x14, e)) return false;
            if (e[0] != id) continue;
            out = cls == 1 ? e[3] : e[1];
            return out != 0;
        }
        return false;
    }

    // current event via the handle at gs+0xB974; returns false when no event is running
    static bool GetEvent(std::uintptr_t gs, std::uint32_t& eventId, std::uintptr_t& data, int& category)
    {
        category = -1;
        data = 0;
        std::uint8_t inEvent = 0;
        std::uint32_t handle = 0;
        std::uintptr_t table = 0;
        if (!Rd(gs + 0xB971, inEvent) || !inEvent) return false;
        if (!Rd(gs + 0xB974, handle) || handle == 0xFFFFFFFFu) return false;
        if (!Rd(gs + 0xB8A0, table) || !table) return false;

        std::uintptr_t blocks = 0, block = 0, idxTable = 0, recs = 0;
        std::uint8_t idx = 0;
        if (!Rd(table + 0xC, blocks) || !blocks) return false;
        if (!Rd(blocks + ((handle >> 8) & 0xFFFF) * 4, block) || !block) return false;
        if (!Rd(block + 0x3C, idxTable) || !idxTable || !Rd(idxTable + (handle & 0xFF), idx)) return false;
        if (!Rd(block + 0x2C, recs) || !recs) return false;
        if (!Rd(recs + idx * 0x120 + 0x38, eventId) || static_cast<std::int32_t>(eventId) < 1) return false;

        if (FindEventData(gs, eventId, data))
        {
            std::uint8_t c = 0;
            if (Rd(data + 0xEC, c)) category = c;
        }
        return true;
    }

    // one line for the event state, one more with the first 0x100 bytes of the event data record
    static void LogEvent(const char* tag)
    {
        auto* game = *reinterpret_cast<std::uint8_t**>(kGamePtr);
        if (!game) return;
        const std::uintptr_t gs = reinterpret_cast<std::uintptr_t>(game) + kGameStateOff;

        std::uint64_t id = 0; std::int32_t idx = 0, cls = 0;
        Rd(gs + 0x48CF8, id);
        Rd(gs + 0x45480, idx);
        Rd(gs + 0x33710, cls);

        std::uint32_t eventId = 0; std::uintptr_t data = 0; int category = -1;
        const bool inEvent = GetEvent(gs, eventId, data, category);

        Logger::Log(std::format("[{}] event {} id {} category {} | vehicle id {} class {} | record {} | active id {}",
                                tag, inEvent ? "yes" : "no", eventId, category, id, cls, idx,
                                DetectActiveCar::GetActiveVehicleId()));

        std::uint32_t d[64] = {};
        if (data && Rd(data, d))
        {
            std::string s;
            for (int i = 0; i < 64; ++i) s += std::format("{:x} ", d[i]);
            Logger::Log(std::format("[{}] event data: {}", tag, s));
        }
    }

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
            changes++;
        }
        Logger::Log(std::format("ProfileDiff: {} changed dwords", changes));

        Position pos;
        if (GetPosition(pos))
        {
            if (s_snapPosValid)
            {
                const float dx = pos.x - s_snapPos.x, dy = pos.y - s_snapPos.y, dz = pos.z - s_snapPos.z;
                Logger::Log(std::format("ProfileDiff: position pos {:.1f}, {:.1f}, {:.1f} (moved {:.1f})",
                                        pos.x, pos.y, pos.z, std::sqrt(dx*dx + dy*dy + dz*dz)));
            }
            else
                LogPosition("diff");
            s_snapPos = pos;
            s_snapPosValid = true;
        }

        s_snap = std::move(now);   // re-arm for the next action
    }

    static void DumpBikeEvents()
    {
        const auto game    = *reinterpret_cast<std::uintptr_t*>(kGamePtr);
        const auto manager = game + 0x6A7A10;
        const auto profile = manager + 0x170;

        const auto count = *reinterpret_cast<std::int32_t*>(manager + 0x420);
        Logger::Log(std::format("BikeEvents: count={}", count));
        if (count < 0 || count > 64)
            return;   // not what we think it is, don't walk it

        for (std::int32_t i = 0; i < count; ++i)
        {
            const auto rec   = manager + 0x77A0 + i * 8;
            const auto entry = profile + (i + 0xEC6) * 8;
            Logger::Log(std::format("BikeEvents: [{}] table id={} aux={:#x} | entry id={} flags={:#06x}",
                i,
                *reinterpret_cast<std::uint32_t*>(rec),
                *reinterpret_cast<std::uint32_t*>(rec + 4),
                *reinterpret_cast<std::uint32_t*>(entry),
                *reinterpret_cast<std::uint16_t*>(entry + 4)));
        }
    }

    static void Loop()
    {
        bool f6Prev = false, f7Prev = false, f8Prev = false, f9Prev = false;
        while (s_running)
        {
            bool fg = GameIsForeground();

            bool f6 = fg && (GetAsyncKeyState(VK_F6) & 0x8000);
            // if (f6 && !f6Prev) { LogPosition("F6"); LogGuiPlayerInfo("F6"); LogRaceCars("F6"); LogEvent("F6"); }
            if (f6 && !f6Prev) { DumpBikeEvents(); }
            f6Prev = f6;

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
