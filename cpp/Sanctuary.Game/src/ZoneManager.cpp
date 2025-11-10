#include <Sanctuary/Game/ZoneManager.h>
#include <spdlog/spdlog.h>
#include <fstream>

namespace Sanctuary::Game {

ZoneManager::ZoneManager() {
    spdlog::info("ZoneManager created");
}

ZoneManager::~ZoneManager() = default;

bool ZoneManager::load_zones(const std::string& zones_file) {
    try {
        std::ifstream file(zones_file);
        if (!file.is_open()) {
            spdlog::error("Failed to open zones file: {}", zones_file);
            return false;
        }

        nlohmann::json json;
        file >> json;

        for (const auto& zone_json : json["zones"]) {
            ZoneDefinition zone;
            zone.id = zone_json["id"];
            zone.name = zone_json["name"];
            zone.internal_name = zone_json.value("internal_name", "");
            zone.spawn_x = zone_json.value("spawn_x", 0.0f);
            zone.spawn_y = zone_json.value("spawn_y", 0.0f);
            zone.spawn_z = zone_json.value("spawn_z", 0.0f);
            zone.max_players = zone_json.value("max_players", 100);

            register_zone(std::move(zone));
        }

        spdlog::info("Loaded {} zones from {}", zones_.size(), zones_file);
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error loading zones: {}", e.what());
        return false;
    }
}

const ZoneDefinition* ZoneManager::get_zone(uint32_t zone_id) const {
    auto it = zones_.find(zone_id);
    return it != zones_.end() ? &it->second : nullptr;
}

const ZoneDefinition* ZoneManager::get_zone_by_name(const std::string& name) const {
    auto it = zone_name_map_.find(name);
    if (it != zone_name_map_.end()) {
        return get_zone(it->second);
    }
    return nullptr;
}

std::vector<const ZoneDefinition*> ZoneManager::get_all_zones() const {
    std::vector<const ZoneDefinition*> result;
    result.reserve(zones_.size());

    for (const auto& [id, zone] : zones_) {
        result.push_back(&zone);
    }

    return result;
}

bool ZoneManager::zone_exists(uint32_t zone_id) const {
    return zones_.find(zone_id) != zones_.end();
}

void ZoneManager::register_zone(ZoneDefinition zone) {
    auto id = zone.id;
    zone_name_map_[zone.name] = id;
    zones_[id] = std::move(zone);
}

} // namespace Sanctuary::Game
