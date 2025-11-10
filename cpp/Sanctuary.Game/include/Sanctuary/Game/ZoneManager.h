#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>

namespace Sanctuary::Game {

struct ZoneDefinition {
    uint32_t id{0};
    std::string name;
    std::string internal_name;
    float spawn_x{0.0f};
    float spawn_y{0.0f};
    float spawn_z{0.0f};
    float spawn_rotation{0.0f};
    uint32_t max_players{100};
    bool is_instance{false};
    bool pvp_enabled{false};
};

class ZoneManager {
public:
    ZoneManager();
    ~ZoneManager();

    // Load zone definitions from file
    bool load_zones(const std::string& zones_file);

    // Zone lookup
    const ZoneDefinition* get_zone(uint32_t zone_id) const;
    const ZoneDefinition* get_zone_by_name(const std::string& name) const;

    // Get all zones
    std::vector<const ZoneDefinition*> get_all_zones() const;

    // Validate zone exists
    bool zone_exists(uint32_t zone_id) const;

private:
    std::unordered_map<uint32_t, ZoneDefinition> zones_;
    std::unordered_map<std::string, uint32_t> zone_name_map_;

    void register_zone(ZoneDefinition zone);
};

} // namespace Sanctuary::Game
