#pragma once

#include <cstdint>
#include <string>
#include <chrono>

namespace Sanctuary::Database::Entities {

struct DbUser {
    uint64_t id{0};
    std::string username;
    std::string password_hash;
    std::string email;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point last_login;
    bool is_admin{false};
    bool is_banned{false};
    std::string ban_reason;
};

} // namespace Sanctuary::Database::Entities
