#pragma once

#include "net_bridge.hpp"
#include <string>

//#define CERT_STORE "cacert.pem"
//#define UUID_FILE "uuid" // TODO: place in %appdata%


#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)

static constexpr const char* GAME_NAME = STRINGIFY(BPRAP_GAME_NAME);

class APClient; 

class ArchepelagoNet
{
public:
	ArchepelagoNet(NetworkBridge& bridge);
	 ~ArchepelagoNet();
	 
	void Run();
	void Stop();
	std::atomic_bool running_{false};

	//static std::string GetSaveIdentifier();
	//static std::string GetItemDesc(int player);
	//static std::string GetItemName(int64_t id, int player);
	//static std::string GetPlayerName(int player);
	//static std::string GetLocationName(int64_t id, int player);
	//static bool ScoutLocations(std::list<int64_t> locations, int create_as_hint = 0);

	std::string uuid;
	bool polling;

	
private:
	NetworkBridge& bridge_;
	std::unique_ptr<APClient> client_;

	void do_connect(const std::string &server, const std::string &slot, const std::string &password);
    void do_disconnect();

	std::atomic<bool> connected{false};
	std::atomic<bool> deathlink_allowed_{false};
	std::string seed;
	int session_slot{-1};   
	std::string slotname;
	int last_item_index_{-1}; 	
};
