#pragma once

#include "bpr/core/ap/slot_data.hpp"
#include <stdint.h>
#include <string>
#include <variant>

namespace NetEvents{
    struct Connected
    {
        std::string seed;
        int slot;
        SlotData slot_data;
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
}

using NetworkEvent = std::variant<
        NetEvents::Connected,
        NetEvents::Disconnected,
        NetEvents::ItemReceived,
        NetEvents::DeathLinkReceived,
        NetEvents::ApConnectionRefused
    >;