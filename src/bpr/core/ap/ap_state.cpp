#include "ap_state.hpp"
#include "bpr/core/events.hpp"
#include "bpr/core/vehicles.hpp"
#include "bpr/hooks/function/detours.hpp"
#include "bpr/net/net_bridge.hpp"
#include <algorithm>
#include <cstdint>
#include "../../app/app.hpp"
#include "../../hooks/game_hooks.hpp"
#include "../../hooks/structure/detours.hpp"
#include "bpr/core/logger.hpp"

ApState::ApState(NetworkBridge& bridge) : bridge_(bridge){
}

void ApState::Update(void* gameActionQueue){
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

                    if(slot_data_.deathlink)
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
                    Logger::Log(std::format("Priming a deathlink from {}, caused by {}", e.source, e.cause));
                    death_link_time = std::chrono::steady_clock::now();
                    g_ProcessingDeathlink = true;
                    DeathLink::KillPlayer();
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
        while (auto item = PopItem()){
            ProcessItem(item->item_id, item->index, gameActionQueue);
        }
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

void ApState::ProcessItem(int64_t item_id, int index, void* gameActionQueue){
    Logger::Log(std::format("Processing item with id {} at index {}, last index is at {}", item_id, index, save_data_.lastIndex_));

    // Area Breakables
    if (slot_data_.lockBreakables == 1 && item_id >= 1000 && item_id < 1010){
        auto area_id = item_id - 1000;
        Logger::Log(std::format("Received {} Breakables - area_id: {}", Map::areaIndex[area_id], area_id));
        for (int i = 0; i < Map::typeIndex.size(); ++i) {
            save_data_.breakable_owned[area_id][i] = true;
            for (const int &location_id : save_data_.deferred_breakables[area_id][i]) {
                SendLocation(location_id + save_data_.breakable_counts[area_id][i]);
                save_data_.breakable_counts[area_id][i]++;
            }
            save_data_.deferred_breakables[area_id][i].clear();
        }
    }

    // Area Breakables per type
    if (slot_data_.lockBreakables == 2 && item_id >= 1010 && item_id < 1100){
        if(item_id == 1035) item_id = 1062;
        if(item_id == 1025) item_id = 1061;
        auto area_id = (item_id - 1010) / 10; // (1050-1010)/10 = (41/10) = 4 -> Downtown
        int type_id = item_id % 10; // single digits match type_id
        Logger::Log(std::format("Received {} {} - area_id: {}, type_id: {}", Map::areaIndex[area_id], Map::typeIndex[type_id], area_id, type_id));

        save_data_.breakable_owned[area_id][type_id] = true;
        for (const int &location_id : save_data_.deferred_breakables[area_id][type_id]) {
            SendLocation(location_id + save_data_.breakable_counts[area_id][type_id]);
            save_data_.breakable_counts[area_id][type_id]++;
        }
        save_data_.deferred_breakables[area_id][type_id].clear();
    }
 
    // Liveries
    if (slot_data_.addedLiveryItems && item_id > 2000 && item_id < 3000){
        const VehicleInfo* info = FindVehicleByArchipelagoLiveryID(item_id);
        Logger::Log(std::format("Received Car Liveries for '{}' - unlock type {}, category {}", info->proper_name, static_cast<int>(info->unlock_type), static_cast<int>(info->category)));
        CarUnlockControl::AddLiveries(item_id);
        save_data_.AddLivery(item_id);
    }

    // Events
    if (item_id > 400000 && item_id < 600000){
        const EventInfo* info = FindByEventId(item_id);
        Logger::Log(std::format("Received Event '{}' - area_id {}, type {}", info->name, static_cast<int>(info->area), static_cast<int>(info->type)));
        EnableEvent::EnableEvent(item_id);
    }

    // Cars
    if (item_id > (uint64_t(0x5) << 48)){
        const VehicleInfo* info = FindVehicleByArchipelagoCarID(item_id);
        Logger::Log(std::format("Received Car '{}' - unlock type {}, category {}", info->proper_name, static_cast<int>(info->unlock_type), static_cast<int>(info->category)));
        CarUnlockControl::AddCar(item_id << 12);
        save_data_.AddCar(item_id << 12);
    }

    // Do not reprocess filler
    if (index < save_data_.lastIndex_){
        return;
    }
    save_data_.lastIndex_++;
    Logger::Log(std::format("Item with id {} at index {} is new, processing it. Last index is now at {}", item_id, index, save_data_.lastIndex_));

    // Filler
    if (item_id == 100){
        Logger::Log(std::format("Received Filler 'Boost Refill'"));
        GameActions::GameAction_SetBoost set_boost{};
        set_boost.Flags.BoostAmount = true;
        set_boost.BoostAmount = 1.0f;
        set_boost.ActiveRaceVehicleIndex = GameHooks::GetPlayerCarIndex();
        GameActions::AddGameAction(gameActionQueue, &set_boost, set_boost.ID, sizeof(set_boost));
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

bool ApState::InDeathTimeout(){
    auto current_time = std::chrono::steady_clock::now();
    return current_time - death_link_time < death_delay;
}
