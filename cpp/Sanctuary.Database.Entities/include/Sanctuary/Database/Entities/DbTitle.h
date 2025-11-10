#pragma once

#include <cstdint>

namespace Sanctuary::Database::Entities {

struct DbTitle {
    uint64_t character_id{0};
    uint32_t title_id{0};
    bool is_unlocked{true};
};

} // namespace Sanctuary::Database::Entities
