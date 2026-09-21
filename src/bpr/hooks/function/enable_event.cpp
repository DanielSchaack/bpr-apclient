#include "bpr/hooks/game_hooks.hpp"
#include "detours.hpp"
#include <exception>
#include <iostream>

struct EventSave{
    uint32_t eventId;
    uint16_t flags;
};


using FindEventInSaveDataFn = EventSave* (__thiscall*)(
    void* self,
    std::uint32_t value
);

auto FindEventSave =  reinterpret_cast<FindEventInSaveDataFn>(0x06e5d2a0);


bool EnableEvent::EnableEvent(uint32_t  event_id) noexcept {
    std::cout << event_id << std::endl;
    EventSave* result = FindEventSave( GameHooks::GetEventSaveManager(), event_id);
    if (result == nullptr){
        std::cout << "event not found" << std::endl;
        return false;
    }

    std::cout << "Event: " << result->eventId << " Flags: " << result->flags << std::endl;
    result->flags = 0x0001;
    return true;
}

bool EnableEvent::IsEventEnabled(uint32_t  event_id) noexcept {
    EventSave* result = FindEventSave( GameHooks::GetEventSaveManager(), event_id);
    if (result == nullptr){
        return false;
    }
    std::cout<< "flags: " << result->flags << std::endl;
    return (result->flags & 1) != 0;
}


bool EnableEvent::IsValidEvent(uint32_t  event_id) noexcept {
    EventSave* result = FindEventSave( GameHooks::GetEventSaveManager(), event_id);
    return result != nullptr;
}