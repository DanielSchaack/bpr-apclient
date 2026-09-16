#include "ap_state.hpp"


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
