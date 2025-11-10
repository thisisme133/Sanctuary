#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Sanctuary::Database::Entities {

struct DbProfile {
    uint64_t character_id{0};
    std::string tag_line;
    std::string bio;
    std::vector<uint32_t> favorite_locations;
    std::vector<uint32_t> favorite_activities;
    uint32_t privacy_flags{0};
};

} // namespace Sanctuary::Database::Entities
