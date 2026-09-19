#include "bpr/hooks/game_hooks.hpp"
#include "detours.hpp"
#include <exception>
#include <iostream>

struct EventSave{
    uint32_t eventId;
    uint32_t flags;
};


using FindEventInSaveDataFn = EventSave* (__thiscall*)(
    void* self,
    std::uint32_t value
);

constexpr std::uintptr_t FindEventInSaveDataAddress = 0x06e5d2a0;

auto FindSomething =  reinterpret_cast<FindEventInSaveDataFn>(FindEventInSaveDataAddress);


bool EnableEvent::EnableEvent(uint32_t  event_id) noexcept {
    std::cout << event_id << std::endl;
    EventSave* result = FindSomething( GameHooks::GetEventSaveManager(), event_id);
    if (result == nullptr){
        std::cout << "event not found" << std::endl;
        return false;
    }

    std::cout << "Event: " << result->eventId << " Flags: " << result->flags << std::endl;
    result->flags |= 32;
    return true;
}

bool EnableEvent::IsEventEnabled(uint32_t  event_id) noexcept {
    EventSave* result = FindSomething( GameHooks::GetEventSaveManager(), event_id);
    if (result == nullptr){
        return false;
    }
    return (result->flags & 32) != 0;
}
