#include "slot_data.hpp"
#include <nlohmann/json.hpp>

bpr::SlotData bpr::parse_slot_data(const nlohmann::json &data){
    bpr::SlotData slot_data;
    const bool obj = data.is_object();
    
    slot_data.licenseLevel = obj ? data.value("licenseLevel", 1) : 1;
    return slot_data;
}
