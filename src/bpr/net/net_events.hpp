#pragma once

#include "bpr/core/ap/slot_data.hpp"
#include <stdint.h>
#include <string>
#include <variant>
#include "../core/broadcast.hpp"

namespace NetEvents{
    struct Connected
    {
        std::string seed;
        int slot;
        bpr::SlotData slot_data;
    };

    struct Disconnected
    {
    };
    struct ApConnectionRefused
    {
        std::vector<std::string> errors;
    };
    struct ItemReceived
    {
        int64_t item_id;
        int index;
        int player;
    };

    struct DeathLinkReceived
    {
        std::string source;
        std::string cause;
    };

    struct ApPrintBroadcast
{
    std::vector<bpr::BannerSegment> segments;
};
}

using NetworkEvent = std::variant<
        NetEvents::Connected,
        NetEvents::Disconnected,
        NetEvents::ItemReceived,
        NetEvents::DeathLinkReceived,
        NetEvents::ApConnectionRefused,
        NetEvents::ApPrintBroadcast
    >;