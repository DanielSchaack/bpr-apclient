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
        Logger::Log(std::format("Event {} not found", result->eventId));
        return false;
    }

    bool isOnBike = DetectActiveCar::IsBikeActive();
    if(result->eventId != current_event)
    {
        Logger::Log(std::format("Check Event {} with flags {}, hex {:x}", result->eventId, result->flags, result->flags));
        current_event = result->eventId;


        if (isOnBike)
            App::Instance->Gui().info_window->AddLogMessage(std::format("Bike Event ID: {}, flags {}, hex {:x}", current_event, result->flags, result->flags));
        else
            App::Instance->Gui().info_window->AddLogMessage(std::format("Car Event ID: {}, flags {}, hex {:x}", current_event, result->flags, result->flags));
    }

    return isOnBike ? App::Instance->State().GetSaveData().HasBikeEvent(event_id) : (result->flags & 1) != 0;
}


bool EnableEvent::IsValidEvent(uint32_t  event_id) noexcept {
    EventSave* result = FindEventSave( GameHooks::GetEventSaveManager(), event_id);
    return result != nullptr;
}

constexpr uint16_t kEventCompletedFlags = 0x0002 | 0x0004;

bool EnableEvent::MarkEventCompleted(uint32_t event_id) noexcept {
    EventSave* result = FindEventSave(GameHooks::GetEventSaveManager(), event_id);
    if (result == nullptr) {
        Logger::Log(std::format("Event {} not found", event_id));
        return false;
    }

    const uint16_t before = result->flags;
    result->flags |= kEventCompletedFlags;
    if (result->flags != before)
        Logger::Log(std::format("Event {} completed flags {:#x} -> {:#x}", event_id, before, result->flags));
    return true;
}

int EnableEvent::RestoreCompletedEvents() noexcept {
    const auto& save = App::Instance->State().GetSaveData();

    int restored = 0;
    for (int i = 0; i<5 ; i++)
    {
        const auto& events = save.completed_events[i];
        for (uint32_t eventId : events)
            if (MarkEventCompleted(eventId))
                ++restored;
    }

    Logger::Log(std::format("Restored completed flags on {} events", restored));
    return restored;
}

