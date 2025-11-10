#pragma once

#include <cstdint>

namespace Sanctuary::Database::Entities {

struct DbMount {
    uint64_t character_id{0};
    uint32_t mount_id{0};
    uint32_t tint_id{0};
    bool is_unlocked{true};
};

} // namespace Sanctuary::Database::Entities
