#include "MinHook.h"
#include "bpr/core/logger.hpp"
#include "bpr/app/app.hpp"
#include <algorithm>
#include <format>
namespace DetectBikeEvent {

    // Thunk: JMP FUN_02f85db0, 719 callers. (queue, data, id, size) -> bool-ish in EAX.
    constexpr std::uintptr_t Address  = 0x004C0590;
    constexpr std::uint32_t  ActionId = 0xD1;     // event state response
    constexpr std::uint32_t  Size     = 0x57C;    // 175 * {id, flags} + count

    using PostFn = std::uint32_t (__thiscall*)(void*, void*, std::uint32_t, std::uint32_t);
    static PostFn Original = nullptr;

    struct Entry   { std::uint32_t id; std::uint32_t flags; };
    struct Payload { Entry entries[175]; std::int32_t count; };
    static_assert(sizeof(Payload) == Size);

    // In the bike table at manager+0x77A0 and not yet received
    static bool IsHiddenBikeEvent(std::uint32_t id)
    {
        const auto manager = *reinterpret_cast<std::uintptr_t*>(0x013FC8E0) + 0x6A7A10;
        const auto count   = *reinterpret_cast<std::int32_t*>(manager + 0x420);
        const auto* table  = reinterpret_cast<const Entry*>(manager + 0x77A0);

        const bool isBike = std::any_of(table, table + std::clamp(count, 0, 0x27),
                                        [id](const Entry& e) { return e.id == id; });
        return isBike && !App::Instance->State().GetSaveData().HasBikeEvent(id);
    }

    static void Filter(Payload& p)
    {
        if (p.count <= 0 || p.count > 175)
            return;

        Entry* const first = p.entries;
        Entry* const last  = first + p.count;
        Entry* const kept  = std::remove_if(first, last, [](const Entry& e) { return IsHiddenBikeEvent(e.id); });

        Logger::Log(std::format("EventState: {} entries, hid {}", p.count, last - kept));
        std::fill(kept, last, Entry{});
        p.count = static_cast<std::int32_t>(kept - first);
    }

    std::uint32_t __fastcall Detour(void* queue, void*, void* data, std::uint32_t id, std::uint32_t size)
    {
        if (id == ActionId && size == Size)
            Filter(*static_cast<Payload*>(data));

        return Original(queue, data, id, size);   // keep the real return value
    }

    MH_STATUS Install()
    {
        return MH_CreateHook(reinterpret_cast<void*>(Address),
                             reinterpret_cast<void*>(&Detour),
                             reinterpret_cast<void**>(&Original));
    }
}
