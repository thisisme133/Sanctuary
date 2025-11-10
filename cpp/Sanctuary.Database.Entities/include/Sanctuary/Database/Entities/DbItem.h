#pragma once

#include <cstdint>
#include <string>

namespace Sanctuary::Database::Entities {

struct DbItem {
    uint64_t id{0};
    uint64_t character_id{0};
    uint32_t template_id{0};
    uint32_t stack_size{1};
    uint32_t slot{0};
    bool is_equipped{false};
    uint32_t durability{100};
    uint32_t tint_id{0};
    std::string custom_name;
    uint32_t stats{0}; // Serialized stats blob or ID
};

} // namespace Sanctuary::Database::Entities
