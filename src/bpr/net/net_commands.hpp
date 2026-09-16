#include <stdint.h>
#include <string>
#include <variant>

namespace NetCommands{
    struct SendLocation
    {
        int64_t location_id;
    };

    struct SendDeathLink
    {
    };

    struct SendGoal
    {
    };

    struct Disconnect
    {
    };

    struct Connect
    {
        std::string server;
        std::string slot;
        std::string password;
    };

}

using NetworkCommand = std::variant<
        NetCommands::SendLocation,
        NetCommands::SendDeathLink,
        NetCommands::SendGoal,
        NetCommands::Disconnect,
        NetCommands::Connect
    >;