#include "ap_state.hpp"
#include "bpr/hooks/function/detours.hpp"
#include "bpr/net/net_bridge.hpp"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <ostream>
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
                    Logger::Log(std::format("Received: {}", e.item_id));
                }
                else if constexpr (std::is_same_v<T, NetEvents::DeathLinkReceived>)
                {
                    death_link_time = std::chrono::steady_clock::now();
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

void ApState::SendBreakableLocation(std::uint32_t type, std::uint32_t id, std::uint32_t area){
    
}

void ApState::ProcessItem(int64_t item_id, int index, void* gameActionQueue){
    if (index < save_data_.lastIndex_){
        return;
    }
    save_data_.lastIndex_++;

    if (item_id >= 1000 && item_id < 2000){
        auto area_id = item_id-1000;
        save_data_.owned_areas.insert(save_data_.owned_areas.end(), area_id);
        for (const auto& [type_id, location_id] : save_data_.deferred_breakables[area_id])
        {
            SendLocation(
                location_id +
                save_data_.breakable_counts[area_id][type_id]
            );

            save_data_.breakable_counts[area_id][type_id]++;
        }
        save_data_.deferred_breakables[area_id].clear();
    }

    if (item_id > 400000 && item_id < 600000){
        EnableEvent::EnableEvent(item_id);
    }

    if (item_id > (uint64_t(0xA) << 48)){
        CarUnlockControl::AddCar(item_id << 12);
    }
    if ( item_id == 100){
        GameActions::GameAction_SetBoost set_boost{};
        set_boost.Flags.BoostAmount = true;
        set_boost.BoostAmount = 1.0f;
        set_boost.ActiveRaceVehicleIndex = GameHooks::GetPlayerCarIndex();
        GameActions::AddGameAction(gameActionQueue, &set_boost, set_boost.ID, sizeof(set_boost));
    }
}

void ApState::CacheBreakable(uint32_t area_id, int type_id){
    int64_t loc_id = 10000 + (1000 * area_id) + (100 * type_id);
    if (!slot_data_.lockBreakables || std::ranges::find(save_data_.owned_areas, area_id) != save_data_.owned_areas.end()) {
        SendLocation(loc_id + save_data_.breakable_counts[area_id][type_id]);
        save_data_.breakable_counts[area_id][type_id]++;
        return;
    }
    save_data_.deferred_breakables[area_id].emplace_back(type_id, loc_id);
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
