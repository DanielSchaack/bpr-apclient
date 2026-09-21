#pragma once
#include "net_commands.hpp"
#include "net_events.hpp"
#include <mutex>
#include <optional>
#include <queue>
#include "../core/broadcast.hpp"

class NetworkBridge
{
    
    
    public:

        NetworkBridge() = default;
        ~NetworkBridge() = default;
        
        bpr::BannerQueue& getBannerQueue()
        {
            return banner_queue_;
        }

        void broadcast(std::vector<bpr::BannerSegment> &segs){
            banner_queue_.push(segs);
        }

        void SendToNetwork(NetworkCommand command)
        {
            std::scoped_lock lock(network_mutex_);
            network_commands_.push(std::move(command));
        }

        void SendToGame(NetworkEvent event)
        {
            std::scoped_lock lock(game_mutex_);
            game_events_.push(std::move(event));
        }
        [[nodiscard]]
        std::optional<NetworkCommand> PopNetworkCommand()
        {
            std::scoped_lock lock(network_mutex_);

            if (network_commands_.empty())
                return std::nullopt;

            NetworkCommand command = std::move(network_commands_.front());
            network_commands_.pop();

            return command;
        }

        [[nodiscard]]
        std::optional<NetworkEvent> PopGameEvent()
        {
            std::scoped_lock lock(game_mutex_);

            if (game_events_.empty())
                return std::nullopt;

            NetworkEvent event = std::move(game_events_.front());
            game_events_.pop();

            return event;
        }

    private:
        std::mutex network_mutex_;
        std::queue<NetworkCommand> network_commands_;

        std::mutex game_mutex_;
        std::queue<NetworkEvent> game_events_;

        bpr::BannerQueue banner_queue_;
};