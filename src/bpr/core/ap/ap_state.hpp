#pragma once

#include "bpr/net/net_bridge.hpp"
#include "slot_data.hpp"
#include <vector>

class ApState
{
    enum class ConnectionPhase
    {
        Disconnected,
        Connecting,
        Connected,
        Error
    };
    public:
        explicit ApState(NetworkBridge& bridge) : bridge_(bridge){}


        void SendLocation(int64_t location_id);
        void SendBreakableLocation(std::uint32_t type, std::uint32_t id, std::uint32_t area);
        void Connect(const std::string &server, const std::string &slot, const std::string &password);
        void Disconnect();
        void Update();
        bool isDisconnected(){
            return phase_ == ConnectionPhase::Disconnected;
        };
        // std::vector<Rec> getRecievedItems();

    private:
        NetworkBridge& bridge_;
        std::atomic<ConnectionPhase> phase_{ConnectionPhase::Disconnected};
        bpr::SlotData SlotData{};
}; 