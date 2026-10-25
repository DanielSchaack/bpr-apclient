#include <windows.h>
#include "MinHook.h"
#include "bpr/core/logger.hpp"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <deque>
#include <format>
#include <mutex>
#include <string>
#include <string_view>

namespace HudNotify
{
    constexpr std::uintptr_t UpdateAddress  = 0x0091AE50;   // HudMessageAnalyzer::Update
    constexpr std::uintptr_t GenericAddress = 0x066E62B0;   // HandleGenericHUDMessage
    using UpdateFn  = void (__thiscall*)(void*, void*, void*);
    using GenericFn = void (__thiscall*)(void*, const void*);
    static UpdateFn Original = nullptr;

    // HandleGeneric builds the message named by the string at 0x00D9E4F8 ("Generic"), a
    // developer message the retail table rejects. Patching the immediate of 066E62BD
    // (MOV ECX, imm32) makes it build another message with our two strings instead.
    // "GenericBad" is the stunt-failure message retail uses (title + reason); the director
    // accepts it (table index 174, available in every mode, no repeat wait).
    constexpr std::uintptr_t kNameOperand  = 0x066E62BE;
    constexpr std::uint32_t  kOriginalName = 0x00D9E4F8;
    static const char        kMessageName[] = "GenericBad";   // static storage, <= 12 chars

    // Exactly what the game reads: title at +0x00, subtitle at +0x40, both < 64 chars.
    struct Payload { char title[64]; char subtitle[64]; };

    static std::mutex g_mutex;
    static std::deque<Payload> g_pending;
    static std::chrono::steady_clock::time_point g_lastShown;

    // Cut at maxBytes (leaving room for "...") without splitting a UTF-8 sequence.
    std::string TruncateUtf8(const std::string& s, std::size_t maxBytes)
    {
        if (s.size() <= maxBytes)
            return s;
        std::size_t cut = maxBytes > 3 ? maxBytes - 3 : 0;
        while (cut > 0 && (static_cast<unsigned char>(s[cut]) & 0xC0) == 0x80)
            --cut;
        return s.substr(0, cut) + "...";
    }

    // Default argument for subtitle lives in detours.hpp.
    void Show(std::string_view title, std::string_view subtitle)
    {
        const std::string t = TruncateUtf8(std::string(title), sizeof(Payload::title) - 1);
        const std::string s = TruncateUtf8(std::string(subtitle), sizeof(Payload::subtitle) - 1);

        Payload p{};
        std::memcpy(p.title, t.data(), t.size());
        std::memcpy(p.subtitle, s.data(), s.size());

        std::lock_guard lock(g_mutex);
        if (g_pending.size() < 16)
            g_pending.push_back(p);
        Logger::Log(std::format("HudNotify: queued '{}' / '{}'", t, s));
    }

    static void PatchMessageName()
    {
        auto* operand = reinterpret_cast<std::uint32_t*>(kNameOperand);
        if (*operand != kOriginalName)
        {
            Logger::Log(std::format("HudNotify: name operand is {:#x}, expected {:#x}, not patching",
                                    *operand, kOriginalName));
            return;
        }
        DWORD old = 0;
        VirtualProtect(operand, sizeof(*operand), PAGE_EXECUTE_READWRITE, &old);
        *operand = reinterpret_cast<std::uint32_t>(kMessageName);
        VirtualProtect(operand, sizeof(*operand), old, &old);
        FlushInstructionCache(GetCurrentProcess(), operand, sizeof(*operand));
        Logger::Log(std::format("HudNotify: message name patched to '{}'", kMessageName));
    }

    // One message at a time: the director rate-gates repeats of the same message.
    static void Drain(void* analyzer, void* context)
    {
        const auto now = std::chrono::steady_clock::now();
        if (now - g_lastShown < std::chrono::milliseconds(3500))
            return;

        Payload p;
        {
            std::lock_guard lock(g_mutex);
            if (g_pending.empty()) return;
            p = g_pending.front();
            g_pending.pop_front();
        }
        g_lastShown = now;

        // Update sets this+8 = ctx for the duration of the call and clears it afterwards.
        auto* slot = reinterpret_cast<void**>(static_cast<std::uint8_t*>(analyzer) + 8);
        void* saved = *slot;
        *slot = context;
        reinterpret_cast<GenericFn>(GenericAddress)(analyzer, &p);
        *slot = saved;

        Logger::Log(std::format("HudNotify: sent '{}' / '{}'", p.title, p.subtitle));
    }

    // ECX = analyzer. EDX is unused by the original __thiscall function.
    // The remaining two arguments stay on the stack, exactly as in the game.
    void __fastcall Detour(void* analyzer, void*, void* queue, void* context)
    {
        static int calls = 0;
        const void* cache = *reinterpret_cast<void**>(static_cast<std::uint8_t*>(analyzer) + 4);
        if (calls++ % 3600 == 0)
            Logger::Log(std::format("HudNotify: Update #{} this={} cache={} pending={}",
                                    calls, analyzer, cache, g_pending.size()));
        if (cache)
            Drain(analyzer, context);
        Original(analyzer, queue, context);
    }

    MH_STATUS Install()
    {
        const MH_STATUS status = MH_CreateHook(
            reinterpret_cast<void*>(UpdateAddress),
            reinterpret_cast<void*>(&Detour),
            reinterpret_cast<void**>(&Original)
        );
        Logger::Log(std::format("HudNotify::Install: Update hook -> {}", MH_StatusToString(status)));
        if (status != MH_OK)
            return status;

        PatchMessageName();
        return MH_OK;
    }
}
