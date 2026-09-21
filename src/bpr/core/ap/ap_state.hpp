#pragma once

#include "bpr/net/net_bridge.hpp"
#include "slot_data.hpp"
#include "../save/save_data.hpp"

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
        ApState(NetworkBridge& bridge);


        void SendLocation(int64_t location_id);
        void SendBreakableLocation(std::uint32_t type, std::uint32_t id, std::uint32_t area);
        void Connect(const std::string &server, const std::string &slot, const std::string &password);
        void Disconnect();
        void Update(void* gameActionQueue);
        void ProcessItem(int64_t item_id, int index, void* gameActionQueue);
        void CacheBreakable(int64_t loc_id, uint32_t area_id);
        void SendGoal();

        [[nodiscard]] bool HasApSaveData() const{
            return save_data_initialized;
        }

        void MarkSaveAsInitialized(){
            save_data_initialized = true;
        }

        [[nodiscard]] bpr::SaveData& GetSaveData() noexcept
        {
            return save_data_;
        }

        [[nodiscard]] bpr::SlotData& GetSlotData() noexcept
        {
            return slot_data_;
        }


        [[nodiscard]] std::string GetSeed() const noexcept
        {
            return save_data_.seed_;
        }

        [[nodiscard]] int GetSlot() const noexcept
        {
            return save_data_.slot_;
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
        bool save_data_initialized = false;
        bpr::SaveData save_data_{};

        std::mutex item_mutex_;
        std::queue<NetEvents::ItemReceived> queued_items_;
}; 