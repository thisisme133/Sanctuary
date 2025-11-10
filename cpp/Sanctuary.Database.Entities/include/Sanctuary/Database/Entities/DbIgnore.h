#pragma once

#include <cstdint>
#include <chrono>

namespace Sanctuary::Database::Entities {

struct DbIgnore {
    uint64_t character_id{0};
    uint64_t ignored_character_id{0};
    std::chrono::system_clock::time_point created_at;
};

} // namespace Sanctuary::Database::Entities
