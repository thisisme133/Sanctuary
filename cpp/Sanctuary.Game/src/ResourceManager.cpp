#include <Sanctuary/Game/ResourceManager.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <filesystem>

namespace Sanctuary::Game {

ResourceManager::ResourceManager() {
    spdlog::info("ResourceManager created");
}

ResourceManager::~ResourceManager() = default;

bool ResourceManager::load_resources(const std::string& resources_dir) {
    namespace fs = std::filesystem;

    if (!fs::exists(resources_dir)) {
        spdlog::error("Resources directory does not exist: {}", resources_dir);
        return false;
    }

    bool success = true;

    auto items_file = fs::path(resources_dir) / "items.json";
    if (fs::exists(items_file)) {
        success &= load_items(items_file.string());
    }

    auto npcs_file = fs::path(resources_dir) / "npcs.json";
    if (fs::exists(npcs_file)) {
        success &= load_npcs(npcs_file.string());
    }

    spdlog::info("Loaded {} item templates and {} NPC templates",
                 item_templates_.size(), npc_templates_.size());

    return success;
}

const ItemTemplate* ResourceManager::get_item_template(uint32_t item_id) const {
    auto it = item_templates_.find(item_id);
    return it != item_templates_.end() ? &it->second : nullptr;
}

const NpcTemplate* ResourceManager::get_npc_template(uint32_t npc_id) const {
    auto it = npc_templates_.find(npc_id);
    return it != npc_templates_.end() ? &it->second : nullptr;
}

bool ResourceManager::item_exists(uint32_t item_id) const {
    return item_templates_.find(item_id) != item_templates_.end();
}

bool ResourceManager::npc_exists(uint32_t npc_id) const {
    return npc_templates_.find(npc_id) != npc_templates_.end();
}

bool ResourceManager::load_items(const std::string& items_file) {
    try {
        std::ifstream file(items_file);
        if (!file.is_open()) {
            spdlog::warn("Failed to open items file: {}", items_file);
            return false;
        }

        nlohmann::json json;
        file >> json;

        for (const auto& item_json : json["items"]) {
            ItemTemplate item;
            item.id = item_json["id"];
            item.name = item_json["name"];
            item.description = item_json.value("description", "");
            item.icon_id = item_json.value("icon_id", 0);
            item.model_id = item_json.value("model_id", 0);
            item.max_stack = item_json.value("max_stack", 1);

            item_templates_[item.id] = std::move(item);
        }

        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error loading items: {}", e.what());
        return false;
    }
}

bool ResourceManager::load_npcs(const std::string& npcs_file) {
    try {
        std::ifstream file(npcs_file);
        if (!file.is_open()) {
            spdlog::warn("Failed to open NPCs file: {}", npcs_file);
            return false;
        }

        nlohmann::json json;
        file >> json;

        for (const auto& npc_json : json["npcs"]) {
            NpcTemplate npc;
            npc.id = npc_json["id"];
            npc.name = npc_json["name"];
            npc.model_id = npc_json.value("model_id", 0);
            npc.level = npc_json.value("level", 1);

            npc_templates_[npc.id] = std::move(npc);
        }

        return true;

    } catch (const std::exception& e) {
        spdlog::error("Error loading NPCs: {}", e.what());
        return false;
    }
}

} // namespace Sanctuary::Game
