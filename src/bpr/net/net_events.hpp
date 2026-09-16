#pragma once

#include <stdint.h>
#include <string>
#include <variant>

namespace NetEvents{
    struct Connected
    {
        std::string seed;
        std::string slot_name;
        std::string slot_data;
    };

    struct Disconnected
    {
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
        NetEvents::DeathLinkReceived
    >;