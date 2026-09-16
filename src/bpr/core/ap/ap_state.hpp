#pragma once

#include "bpr/net/net_bridge.hpp"
#include "slot_data.hpp"

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
        void Connect(const std::string &server, const std::string &slot, const std::string &password);
        void Disconnect();
        void Update();
    private:
        NetworkBridge& bridge_;
        std::atomic<ConnectionPhase> phase_{ConnectionPhase::Disconnected};
        SlotData SlotData{};
}; 