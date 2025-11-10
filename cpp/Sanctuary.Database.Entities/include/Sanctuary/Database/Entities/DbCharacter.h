#pragma once

#include <cstdint>
#include <string>
#include <chrono>

namespace Sanctuary::Database::Entities {

struct DbCharacter {
    uint64_t id{0};
    uint64_t user_id{0};
    std::string name;
    uint32_t model_id{0};
    uint32_t head_model{0};
    uint32_t hair_model{0};
    uint32_t hair_color{0};
    uint32_t eye_color{0};
    uint32_t skin_tone{0};
    uint32_t face_paint{0};
    uint32_t model_customization{0};
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    float rot{0.0f};
    uint32_t world_id{0};
    uint32_t level{1};
    uint64_t experience{0};
    uint32_t coins{0};
    uint32_t station_cash{0};
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point last_login;
    uint32_t active_title_id{0};
};

} // namespace Sanctuary::Database::Entities
