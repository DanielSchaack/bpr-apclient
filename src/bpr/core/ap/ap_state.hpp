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
        void ProcessItem(uint64_t item_id);

        [[nodiscard]] std::string GetSeed() const noexcept
        {
            return seed_;
        }

        [[nodiscard]] int GetSlot() const noexcept
        {
            return slot_;
        }

        [[nodiscard]] bool isDisconnected() const noexcept
        {
            return phase_.load(std::memory_order_acquire) ==
                ConnectionPhase::Disconnected;
        }

        [[nodiscard]] bool isConnected() const noexcept
        {
            return phase_.load(std::memory_order_acquire) ==
                ConnectionPhase::Connected;
        }

        void QueueItem(NetEvents::ItemReceived item)
        {
            std::scoped_lock lock(item_mutex_);
            queued_items_.push(std::move(item));
        }

        
        [[nodiscard]] std::optional<NetEvents::ItemReceived> PopItem()
        {
            std::scoped_lock lock(item_mutex_);

            if (queued_items_.empty())
                return std::nullopt;

            NetEvents::ItemReceived item = std::move(queued_items_.front());
            queued_items_.pop();

            return item;
        }

        

    private:
        NetworkBridge& bridge_;
        std::atomic<ConnectionPhase> phase_{ConnectionPhase::Disconnected};
        bpr::SlotData slot_data_{};
        std::string seed_;
        int slot_;

        std::mutex item_mutex_;
        std::queue<NetEvents::ItemReceived> queued_items_;
}; 