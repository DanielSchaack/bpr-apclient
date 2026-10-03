#include "bpr/app/app.hpp"
#include "bpr/hooks/game_hooks.hpp"
#include "detours.hpp"
#include "bpr/core/logger.hpp"
#include <cstdint>
#include <format>

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
    EventSave* result = FindEventSave( GameHooks::GetEventSaveManager(), event_id);
    if (result == nullptr){
        Logger::Log(std::format("Event {} not found", event_id));
        return false;
    }

    Logger::Log(std::format("Enabled Event {} with flags {}", result->eventId, result->flags));
    result->flags |= 0x0001;
    return true;
}

uint32_t current_event = 0;
bool EnableEvent::IsEventEnabled(uint32_t  event_id) noexcept {
    EventSave* result = FindEventSave( GameHooks::GetEventSaveManager(), event_id);
    if (result == nullptr){
        return false;
    }

    if(result->eventId != current_event)
    {
        Logger::Log(std::format("Check Event {} with flags {}", result->eventId, result->flags));
        current_event = result->eventId;


        if (DetectActiveCar::IsBikeActive())
            App::Instance->Gui().info_window->AddLogMessage(std::format("Bike Event ID: {}", current_event));
    }
    return (result->flags & 1) != 0;
}


bool EnableEvent::IsValidEvent(uint32_t  event_id) noexcept {
    EventSave* result = FindEventSave( GameHooks::GetEventSaveManager(), event_id);
    return result != nullptr;
}
