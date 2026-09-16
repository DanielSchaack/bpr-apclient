#include "ap_net.hpp"

#include <apclient.hpp>
#include <iostream>
// #include <apuuid.hpp>

ArchepelagoNet::ArchepelagoNet(NetworkBridge& bridge) : bridge_(bridge) {
}
ArchepelagoNet::~ArchepelagoNet() = default;

void ArchepelagoNet::Run() {
    running_ = true;
	while (running_) {
        //handle every single command
        while (auto command = bridge_.PopNetworkCommand()){
            std::visit(
                [this](auto&& cmd)
                {
                    using T = std::decay_t<decltype(cmd)>;

                    if constexpr (std::is_same_v<T, NetCommands::SendLocation>)
                    {
                        if (ap)
                            ap->LocationChecks({cmd.location_id});
                    }
                    else if constexpr (std::is_same_v<T, NetCommands::SendDeathLink>)
                    {
                        // send deathlink
                    }
                    else if constexpr (std::is_same_v<T, NetCommands::SendGoal>)
                    {
                        if (ap)
                            ap->StatusUpdate(APClient::ClientStatus::GOAL);
                    }
                    else if constexpr (std::is_same_v<T, NetCommands::Disconnect>)
                    {
                        do_disconnect();
                    }else if constexpr (std::is_same_v<T, NetCommands::Connect>) {
                        do_connect(cmd.server, cmd.slot, cmd.password);
                    }
                },
                *command
            );
        }
		if (ap && polling) {
			ap->poll();
		}
	}
}

void ArchepelagoNet::Stop()
{
    running_ = false;
}

void ArchepelagoNet::do_connect(const std::string &server, const std::string &slot, const std::string &password)
{
    do_disconnect();
    std::cout << "Try Connecting!!!" << std::endl;
    return;
}

void ArchepelagoNet::do_disconnect()
{
    if (!ap)
        return;
    ap.reset();
}