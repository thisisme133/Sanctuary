#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>

namespace Sanctuary::Game {

struct ItemTemplate {
    uint32_t id{0};
    std::string name;
    std::string description;
    uint32_t icon_id{0};
    uint32_t model_id{0};
    uint32_t max_stack{1};
    uint32_t item_type{0};
    uint32_t rarity{0};
    uint32_t level_requirement{0};
    bool tradeable{true};
    bool consumable{false};
};

struct NpcTemplate {
    uint32_t id{0};
    std::string name;
    uint32_t model_id{0};
    uint32_t level{0};
    uint32_t faction_id{0};
    bool is_hostile{false};
};

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    // Load resources from directory
    bool load_resources(const std::string& resources_dir);

    // Item template lookup
    const ItemTemplate* get_item_template(uint32_t item_id) const;

    // NPC template lookup
    const NpcTemplate* get_npc_template(uint32_t npc_id) const;

    // Validate resources
    bool item_exists(uint32_t item_id) const;
    bool npc_exists(uint32_t npc_id) const;

private:
    std::unordered_map<uint32_t, ItemTemplate> item_templates_;
    std::unordered_map<uint32_t, NpcTemplate> npc_templates_;

    bool load_items(const std::string& items_file);
    bool load_npcs(const std::string& npcs_file);
};

} // namespace Sanctuary::Game
