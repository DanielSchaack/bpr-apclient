#include "ap_state.hpp"
#include "bpr/core/broadcast.hpp"
#include "bpr/core/events.hpp"
#include "bpr/core/map.hpp"
#include "bpr/core/mod_config.hpp"
#include "bpr/core/vehicles.hpp"
#include "bpr/hooks/function/detours.hpp"
#include "bpr/net/net_bridge.hpp"
#include <algorithm>
#include <cstdint>
#include <format>
#include "../../app/app.hpp"
#include "../../hooks/game_hooks.hpp"
#include "../../hooks/structure/detours.hpp"
#include "bpr/core/logger.hpp"

ApState::ApState(NetworkBridge& bridge) : bridge_(bridge){
}

void ApState::Update(void* gameActionQueue, void* gameEventQueue){
    //handle every single event
    while (auto event = bridge_.PopGameEvent())
    {
        std::visit(
            [this](auto&& e)
            {
                using T = std::decay_t<decltype(e)>;

                if constexpr (std::is_same_v<T, NetEvents::Connected>)
                {
                    App::Instance->Gui().login_window->SetMessage("Connected");
                    save_data_ = bpr::SaveData{
                        .seed_ = std::move(e.seed),
                        .slot_ = e.slot
                    };
                    slot_data_ = std::move(e.slot_data);

                    if(ModConfig::Get().deathlink)
                        death_link_time = std::chrono::steady_clock::now();

                    phase_.store(ConnectionPhase::Connected);
                }
                else if constexpr (std::is_same_v<T, NetEvents::Disconnected>)
                {
                    phase_.store(ConnectionPhase::Disconnected);
                    App::Instance->Gui().login_window->SetMessage("Disconnected");
                    Logger::Log("AP Diconnect");
                }
                else if constexpr (std::is_same_v<T, NetEvents::ItemReceived>)
                {
                    QueueItem(e);
                    Logger::Log(std::format("Received: {}, index: {}", e.item_id, e.index));
                }
                else if constexpr (std::is_same_v<T, NetEvents::DeathLinkReceived>)
                {
                    if (ModConfig::Get().deathlink)
                    {
                        std::vector<bpr::BannerSegment> segments;
                        std::string msg = std::format("Deathlink from {}: {}", e.source, bpr::deathlink_banner_text(e.source, e.cause));
                        Logger::Log(msg);
                        segments.push_back(bpr::BannerSegment(msg, 0xFFFFFFFF));
                        bridge_.broadcast(segments);

                        death_link_time = std::chrono::steady_clock::now();
                        g_ProcessingDeathlink = true;
                        DeathLink::KillPlayer();
                    }
                }
                else if constexpr (std::is_same_v<T, NetEvents::ApPrintBroadcast>)
                {
                    bridge_.broadcast(e.segments);
                }
            },
            *event
        );
    }

    if (GameHooks::GetCurrentGameStateFlag() == 6 && GameHooks::isInGame()){
        if(!g_setupDone)
        {
            for(const Map::DriveThruInfo info : Map::driveThruInfos)
                DetectDriveThru::RevokeDiscovery(info.id);

            for(const auto drivethru_id : save_data_.visited_drivethrus)
                DetectDriveThru::GrantDiscovery(drivethru_id);

            if(slot_data_.finishedEventsStayFinished)
                EnableEvent::RestoreCompletedEvents();

            EventWinLog::ApplyBaseDifficulty(slot_data_.modeBaseDifficulty);

            while (auto item = PopItem()){
                ProcessItem(item->item_id, item->index, gameActionQueue, gameEventQueue);
            }

            g_setupDone = true;
        }

        while (auto item = PopItem()){
            ProcessItem(item->item_id, item->index, gameActionQueue, gameEventQueue);
        }

        Filler::Tick(gameActionQueue, gameEventQueue);
    }
}


void ApState::Connect(const std::string &server, const std::string &slot, const std::string &password){
    if (phase_ == ConnectionPhase::Disconnected){
        phase_ = ConnectionPhase::Connecting;
        bridge_.SendToNetwork(NetCommands::Connect{server, slot, password});
    }
}

void ApState::Disconnect(){
    if (phase_ == ConnectionPhase::Connected){
        bridge_.SendToNetwork(NetCommands::Disconnect{});
    }
}

void ApState::SendLocation(int64_t location_id){
    Logger::Log(std::format("Try Send Location: {}", location_id));
    if (phase_ == ConnectionPhase::Connected){
        bridge_.SendToNetwork(NetCommands::SendLocation{location_id});
    }
}

void ApState::SendDeathLink(){
    Logger::Log(std::format("Try Send Death"));
    if (phase_ == ConnectionPhase::Connected){
        death_link_time = std::chrono::steady_clock::now();
        bridge_.SendToNetwork(NetCommands::SendDeathLink{});
    }
}

static void RevokeAreaDriveThrus(int area_id) {
    int revoked = 0;
    for (const Map::DriveThruInfo* info : Map::FindAllDriveThrusByArea(area_id)) {
        if (DetectDriveThru::RevokeDiscovery(info->id))
            ++revoked;
    }
    Logger::Log(std::format("Revoked {} drive-thrus in {}", revoked, Map::areaIndex[area_id]));
}

void ApState::ProcessItem(int64_t item_id, int index, void* gameActionQueue, void* gameEventQueue){
    Logger::Log(std::format("Processing item with id {} at index {}, last index is at {}", item_id, index, save_data_.lastIndex_));

    // Area Breakables
    if (slot_data_.lockBreakables == 1 && item_id >= 1000 && item_id < 1010){
        auto area_id = item_id - 1000;
        Logger::Log(std::format("Received {} Breakables - area_id: {}", Map::areaIndex[area_id], area_id));
        for (int i = 0; i < Map::typeIndex.size(); ++i) {
            save_data_.breakable_owned[area_id][i] = true;

            if (i == 3 && index > save_data_.lastIndex_ && !slot_data_.cacheBreakables) {
                RevokeAreaDriveThrus(area_id);
            } else {
                for (const int64_t &location_id : save_data_.deferred_breakables[area_id][i]) {
                    SendLocation(location_id + save_data_.breakable_counts[area_id][i]);
                    save_data_.breakable_counts[area_id][i]++;
                }
            }
            save_data_.deferred_breakables[area_id][i].clear();
        }
    }

    // Area Breakables per type
    if (slot_data_.lockBreakables == 2 && item_id >= 1010 && item_id < 1100){
        auto area_id = (item_id - 1010) / 10;
        int type_id = item_id % 10;
        Logger::Log(std::format("Received {} {} - area_id: {}, type_id: {}", Map::areaIndex[area_id], Map::typeIndex[type_id], area_id, type_id));

        save_data_.breakable_owned[area_id][type_id] = true;

        if (type_id == 3 && index > save_data_.lastIndex_ && !slot_data_.cacheBreakables) {
            RevokeAreaDriveThrus(area_id);
        } else {
            for (const int64_t &location_id : save_data_.deferred_breakables[area_id][type_id]) {
                SendLocation(location_id + save_data_.breakable_counts[area_id][type_id]);
                save_data_.breakable_counts[area_id][type_id]++;
            }
        }
        save_data_.deferred_breakables[area_id][type_id].clear();
    }
 
    // Liveries
    if (slot_data_.addedLiveryItems && item_id > 2000 && item_id < 3000){
        const Vehicle::VehicleInfo* info = Vehicle::FindVehicleByArchipelagoLiveryID(item_id);
        Logger::Log(std::format("Received Car Liveries for '{}' - unlock type {}, category {}", info->proper_name, static_cast<int>(info->unlock_type), static_cast<int>(info->category)));
        CarUnlockControl::AddLiveries(item_id);
        save_data_.AddLivery(item_id);
    }

    if (item_id > 200000 && item_id < 850000){
        // Car Events
        if (const Event::EventInfo* info = Event::FindByEventId(item_id))
        {
            Logger::Log(std::format("Received Car Event '{}' - area_id {}, type {}", info->name, static_cast<int>(info->area), static_cast<int>(info->type)));
            EnableEvent::EnableEvent(item_id);
            save_data_.obtained_car_events.insert(item_id);
        }
        // Drive-Thru Jump points
        else if (const Map::DriveThruInfo* info = Map::FindDriveThruByID(item_id))
        {
            Logger::Log(std::format("Received Drive-Thru Jumppoint '{}' - area_id {}, type {}", info->junction, static_cast<int>(info->area), static_cast<int>(info->type)));
            DetectDriveThru::GrantDiscovery(item_id);
            save_data_.obtained_drivethru_jumppoints.insert(item_id);
        }
    }

    if (item_id > 2000000 && item_id < 8000000){
        // Bike Events
        if (const Event::EventInfo* info = Event::FindByEventId(item_id / 10))
        {
            Logger::Log(std::format("Received Bike Event '{}' - area_id {}, type {}", info->name, static_cast<int>(info->area), static_cast<int>(info->type)));
            save_data_.obtained_bike_events.insert(item_id / 10);
        }
    }

    // Cars
    if (item_id > (uint64_t(0x5) << 48)){
        const Vehicle::VehicleInfo* info = Vehicle::FindVehicleByArchipelagoCarID(item_id);
        Logger::Log(std::format("Received Car '{}' - unlock type {}, category {}", info->proper_name, static_cast<int>(info->unlock_type), static_cast<int>(info->category)));
        save_data_.AddCar(item_id << 12);
        CarUnlockControl::AddCar(item_id << 12);

        if(ModConfig::Get().useWhatYouGet)
            Filler::SwapToCar(gameEventQueue, info->id);

        if(!ModConfig::Get().checkGoalOnlyOnEventAndLicense)
            CheckGoal();
    }

    if (index > save_data_.lastIndex_)
        save_data_.lastIndex_ = index;

    // Do not reprocess filler
    if (index < save_data_.lastIndex_ || !g_setupDone)
        return;

    Logger::Log(std::format("Item with id {} is new, processing it. Last index is now at {}", item_id, index, save_data_.lastIndex_));

    // Filler
    if (item_id >= 100 and item_id <= 200) {
        Filler::Handle(item_id, gameActionQueue, gameEventQueue);
    }

    Logger::Log(std::format("Done processing item {}", item_id));
}

void ApState::CacheBreakable(uint32_t area_id, int type_id){
    int64_t loc_id = 10000 + (1000 * area_id) + (100 * type_id);
    if (!slot_data_.lockBreakables || save_data_.breakable_owned[area_id][type_id]) {
        SendLocation(loc_id + save_data_.breakable_counts[area_id][type_id]);
        save_data_.breakable_counts[area_id][type_id]++;
        return;
    }

    Logger::Log(std::format("Caching {} for area {}", Map::typeIndex[type_id], Map::areaIndex[area_id]));
    save_data_.deferred_breakables[area_id][type_id].push_back(loc_id);
}

void ApState::SendGoal(){
    Logger::Log("Send Goal");
    if (phase_ == ConnectionPhase::Connected){
        bridge_.SendToNetwork(NetCommands::SendGoal{});
    }
}

bool HasEnoughEventWins(Event::EventType type)
{
    auto& state = App::Instance->State();

    const auto wins     = state.GetSaveData().GetEventWins(type);
    const auto required = state.GetSlotData().uniqueEventWinsAmounts[static_cast<uint8_t>(type)];

    Logger::Log(std::format("Has enough {} wins: {} >= {}", Event::to_string(type), wins, required));

    return wins >= required;
}

void ApState::CheckGoal(){
    Logger::Log("Checking Goal");

    bool has_license = App::Instance->State().GetSaveData().license >= App::Instance->State().GetSlotData().licenseGoal;
    Logger::Log(std::format("Has license goal met: {} >= {}", App::Instance->State().GetSaveData().license, App::Instance->State().GetSlotData().licenseGoal));

    bool has_cars = App::Instance->State().GetSaveData().obtained_paradise_cars.size() >= App::Instance->State().GetSlotData().carGoalCount;
    Logger::Log(std::format("Has enough paradise cars: {} >= {}", App::Instance->State().GetSaveData().obtained_paradise_cars.size(), App::Instance->State().GetSlotData().carGoalCount));

    bool has_burning_route_wins = HasEnoughEventWins(Event::EventType::BURNING_ROUTE);
    bool has_race_wins = HasEnoughEventWins(Event::EventType::RACE);
    bool has_stunt_run_wins = HasEnoughEventWins(Event::EventType::STUNT_RUN);
    bool has_road_rage_wins = HasEnoughEventWins(Event::EventType::ROAD_RAGE);
    bool has_marked_man_wins = HasEnoughEventWins(Event::EventType::MARKED_MAN);
    bool has_big_surf_island_wins = HasEnoughEventWins(Event::EventType::BIG_SURF_ISLAND);
    bool has_burning_ride_wins = HasEnoughEventWins(Event::EventType::BURNING_RIDE);
    bool has_midnight_ride_wins = HasEnoughEventWins(Event::EventType::MIDNIGHT_RIDE);

    bool has_wins = has_burning_route_wins && has_race_wins && has_stunt_run_wins && has_road_rage_wins && has_marked_man_wins && has_big_surf_island_wins && has_burning_ride_wins && has_midnight_ride_wins;

    if (has_license && has_cars && has_wins)
        SendGoal();
}

bool ApState::InDeathTimeout(){
    auto current_time = std::chrono::steady_clock::now();
    return current_time - death_link_time < death_delay;
}
