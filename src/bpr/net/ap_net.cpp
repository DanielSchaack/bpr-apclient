#include "ap_net.hpp"
#include "bpr/core/ap/slot_data.hpp"
#include "deathlink.hpp"
#include "net_events.hpp"

#include <apclient.hpp>
#include <iostream>
#include <variant>
#include <apuuid.hpp>

#define UUID_FILE "uuid" // TODO: place in %appdata%

bool is_wss = false;
bool is_ws = false;
constexpr int kItemHandling = 0b111;   

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
                        if (client_ && client_->get_missing_locations().contains(cmd.location_id))
                            client_->LocationChecks({cmd.location_id});
                    }
                    else if constexpr (std::is_same_v<T, NetCommands::SendDeathLink>)
                    {
                        // send deathlink
                    }
                    else if constexpr (std::is_same_v<T, NetCommands::SendGoal>)
                    {
                        if (client_)
                            client_->StatusUpdate(APClient::ClientStatus::GOAL);
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
		if (client_ && polling) {
			client_->poll();
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
    polling = true;
    std::string uuid = ap_get_uuid(UUID_FILE,
	server.empty() ? APClient::DEFAULT_URI :
        is_ws ? server.substr(5) :
        is_wss ? server.substr(6) :
        server);
    std::cout << GAME_NAME << std::endl;
    client_ = std::make_unique<APClient>(uuid, GAME_NAME, server);

    client_->set_slot_connected_handler([this,slot, password](const nlohmann::json& data) {
        slotname = slot;
        bpr::SlotData slot_data = bpr::parse_slot_data(data);
        deathlink_allowed_.store(slot_data.deathlink);

        std::list<std::string> tags;
        if (slot_data.deathlink)
            tags.push_back("DeathLink");
        client_->ConnectUpdate(false, kItemHandling, true, tags);
        client_->StatusUpdate(APClient::ClientStatus::PLAYING);

        const std::string new_seed = client_->get_seed();
        const int new_player_slot = client_->get_player_number();
        if (new_seed != seed || new_player_slot != session_slot)
        {
            seed = new_seed;
            session_slot = new_player_slot;
            last_item_index_ = -1;
            // bridge_.SendToGame(NetEvents::Disconnected{});
        }

        auto missing = client_->get_missing_locations();
        auto checked = client_->get_checked_locations();
        bridge_.SendToGame(NetEvents::Connected{
            .seed = client_->get_seed(),
            .slot = session_slot,
            .slot_data = slot_data
        });
        connected.store(true);
    });
    client_->set_socket_disconnected_handler(
        [this]
        {
            connected.store(false);
            bridge_.SendToGame(NetEvents::Disconnected{});
        });
    client_->set_room_info_handler(
        [this, slot, password]
        {
            std::list<std::string> tags;
            if (deathlink_allowed_.load())
                tags.push_back("DeathLink");
            client_->ConnectSlot(slot, password, kItemHandling, tags);
        });
    client_->set_slot_refused_handler(
        [this](const std::list<std::string> &errors)
        {
            connected.store(false);
            bridge_.SendToGame(NetEvents::ApConnectionRefused{std::vector<std::string>(errors.begin(), errors.end())});
        });
    client_->set_items_received_handler(
        [this](const std::list<APClient::NetworkItem> &items)
        {
            for (const auto &item : items)
            {
                if (item.index <= last_item_index_)
                    continue;
                bridge_.SendToGame(NetEvents::ItemReceived{.item_id = item.item, .index = item.index, .player = item.player});
                last_item_index_ = item.index;
            }
        });
    client_->set_bounced_handler(
        [this](const nlohmann::json &cmd)
        {
            if (!deathlink_allowed_.load())
                return;
            if (auto t = cmd.find("tags"); t == cmd.end() || std::find(t->begin(), t->end(), "DeathLink") == t->end())
                return;
            std::string payload = cmd.contains("data") ? cmd["data"].dump() : std::string{};
            auto dl = bpr_net::parse_deathlink_payload(payload);
            // The server relays a tagged Bounce to every same-team client holding that tag, sender included, so
            // our own death comes back to us; the echo is the only evidence it reached the room. `source` is
            // optional in the parse, so guard on a non-empty slot name, or a sourceless bounce from someone else
            // gets swallowed as our echo.
            if (dl && !slotname.empty() && dl->source == slotname)
            {
                return;
            }
            std::string source = dl ? std::move(dl->source) : std::string{};
            std::string cause = dl ? std::move(dl->cause) : std::string{};
            bridge_.SendToGame(NetEvents::DeathLinkReceived{.source = std::move(source), .cause = std::move(cause)});
        });
    client_->set_print_json_handler(
        [this](const APClient::PrintJSONArgs &args)
        {
            const auto opt = [](const int *p) { return p ? std::optional<int>(*p) : std::nullopt; };
            // args.item->player is the finder: relevant when we sent the check (item destined for another slot).
            const std::optional<int> item_player = args.item ? std::optional<int>(args.item->player) : std::nullopt;
            if (!bpr::broadcast_relevant(args.type, client_->get_team_number(), client_->get_player_number(), opt(args.team), opt(args.slot),
                                         opt(args.receiving), item_player))
                return;

            std::vector<bpr::BannerSegment> segments;
            for (const auto &node : args.data)
            {
                std::string text = client_->render_json(std::list<APClient::TextNode>{node}, APClient::RenderFormat::TEXT);
                if (text.empty())
                    continue;
                bool is_self = false;
                if (node.type == "player_id")
                    try
                    {
                        is_self = std::stoi(node.text) == client_->get_player_number();
                    }
                    catch (const std::exception &)
                    {
                    }
                segments.push_back(bpr::BannerSegment{std::move(text), bpr::banner_color(node.type, node.color, node.flags, node.hintStatus, is_self)});
            }
            if (segments.empty())
                return;
            bridge_.SendToGame( NetEvents::ApPrintBroadcast{std::move(segments)});
        });
}

void ArchepelagoNet::do_disconnect()
{
    if (!client_)
        return;
    client_.reset();
    bridge_.SendToGame(NetEvents::Disconnected{});
}