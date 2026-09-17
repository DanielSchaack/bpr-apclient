#include "ap_state.hpp"
#include "bpr/net/net_bridge.hpp"
#include <iostream>


void ApState::Update(){
    //handle every single event
    while (auto event = bridge_.PopGameEvent())
    {
        std::visit(
            [this](auto&& e)
            {
                using T = std::decay_t<decltype(e)>;

                if constexpr (std::is_same_v<T, NetEvents::Connected>)
                {

                }
                else if constexpr (std::is_same_v<T, NetEvents::Disconnected>)
                {

                }
                else if constexpr (std::is_same_v<T, NetEvents::ItemReceived>)
                {

                }
                else if constexpr (std::is_same_v<T, NetEvents::DeathLinkReceived>)
                {

                }
            },
            *event
        );
    }
}


void ApState::Connect(const std::string &server, const std::string &slot, const std::string &password){
    if (phase_ == ConnectionPhase::Disconnected){
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