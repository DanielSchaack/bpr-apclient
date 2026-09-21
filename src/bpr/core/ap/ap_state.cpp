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
                }
                else if constexpr (std::is_same_v<T, NetEvents::ItemReceived>)
                {
                    std::cout << "Recieved: " << e.item_id << std::endl;
                    QueueItem(e);
                }
                else if constexpr (std::is_same_v<T, NetEvents::DeathLinkReceived>)
                {
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
    std::cout << "Try Check: " << location_id << std::endl;
    if (phase_ == ConnectionPhase::Connected){
        bridge_.SendToNetwork(NetCommands::SendLocation{location_id});
    }
}

void ApState::SendBreakableLocation(std::uint32_t type, std::uint32_t id, std::uint32_t area){
    
}

void ApState::ProcessItem(int64_t item_id, int index, void* gameActionQueue){
    if (index < save_data_.lastIndex_){
        return;
    }
    save_data_.lastIndex_++;
    
    std::cout << "Process: " << item_id << std::endl;

    if (item_id >= 1000 && item_id < 2000){
        auto area_id = item_id-1000;
        save_data_.owned_areas.insert(save_data_.owned_areas.end(), area_id);
        std::ranges::for_each(save_data_.defered_breakables[area_id], 
            [&](int64_t location_id)
        {
            SendLocation(location_id + save_data_.breakable_counts[area_id]);
            save_data_.breakable_counts[area_id]++;
        });
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

void ApState::CacheBreakable(int64_t loc_id, uint32_t area_id){
    if (!slot_data_.lockBreakables || std::ranges::find(save_data_.owned_areas, area_id) != save_data_.owned_areas.end()) {
        SendLocation(loc_id + save_data_.breakable_counts[area_id]);
        save_data_.breakable_counts[area_id]++;
        std::cout << "Send Breakable" << std::endl;
        return;
    }
    std::cout << "Defer Breakable" << std::endl;
    save_data_.defered_breakables[area_id].insert(save_data_.defered_breakables[area_id].end(), loc_id);
}

void ApState::SendGoal(){
    std::cout << "Send Goal: " << std::endl;
    if (phase_ == ConnectionPhase::Connected){
        bridge_.SendToNetwork(NetCommands::SendGoal{});
    }
}