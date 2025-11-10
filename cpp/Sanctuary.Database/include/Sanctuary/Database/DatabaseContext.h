#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <expected>
#include <span>
#include <Sanctuary/Database/Entities/DbUser.h>
#include <Sanctuary/Database/Entities/DbCharacter.h>
#include <Sanctuary/Database/Entities/DbItem.h>
#include <Sanctuary/Database/Entities/DbFriend.h>
#include <Sanctuary/Database/Entities/DbIgnore.h>
#include <Sanctuary/Database/Entities/DbProfile.h>
#include <Sanctuary/Database/Entities/DbMount.h>
#include <Sanctuary/Database/Entities/DbTitle.h>

namespace Sanctuary::Database {

enum class DatabaseError {
    ConnectionFailed,
    QueryFailed,
    NotFound,
    ConstraintViolation
};

class DatabaseContext {
public:
    explicit DatabaseContext(const std::string& connection_string);
    ~DatabaseContext();

    // User operations
    std::expected<Entities::DbUser, DatabaseError> get_user_by_id(uint64_t user_id);
    std::expected<Entities::DbUser, DatabaseError> get_user_by_username(std::string_view username);
    std::expected<uint64_t, DatabaseError> create_user(const Entities::DbUser& user);
    std::expected<void, DatabaseError> update_user(const Entities::DbUser& user);
    std::expected<void, DatabaseError> delete_user(uint64_t user_id);

    // Character operations
    std::expected<Entities::DbCharacter, DatabaseError> get_character_by_id(uint64_t character_id);
    std::expected<std::vector<Entities::DbCharacter>, DatabaseError> get_characters_by_user_id(uint64_t user_id);
    std::expected<uint64_t, DatabaseError> create_character(const Entities::DbCharacter& character);
    std::expected<void, DatabaseError> update_character(const Entities::DbCharacter& character);
    std::expected<void, DatabaseError> delete_character(uint64_t character_id);

    // Item operations
    std::expected<Entities::DbItem, DatabaseError> get_item_by_id(uint64_t item_id);
    std::expected<std::vector<Entities::DbItem>, DatabaseError> get_items_by_character_id(uint64_t character_id);
    std::expected<uint64_t, DatabaseError> create_item(const Entities::DbItem& item);
    std::expected<void, DatabaseError> update_item(const Entities::DbItem& item);
    std::expected<void, DatabaseError> delete_item(uint64_t item_id);

    // Friend operations
    std::expected<std::vector<Entities::DbFriend>, DatabaseError> get_friends_by_character_id(uint64_t character_id);
    std::expected<void, DatabaseError> add_friend(const Entities::DbFriend& friend_entry);
    std::expected<void, DatabaseError> remove_friend(uint64_t character_id, uint64_t friend_id);

    // Ignore operations
    std::expected<std::vector<Entities::DbIgnore>, DatabaseError> get_ignores_by_character_id(uint64_t character_id);
    std::expected<void, DatabaseError> add_ignore(const Entities::DbIgnore& ignore_entry);
    std::expected<void, DatabaseError> remove_ignore(uint64_t character_id, uint64_t ignored_id);

    // Profile operations
    std::expected<Entities::DbProfile, DatabaseError> get_profile_by_character_id(uint64_t character_id);
    std::expected<void, DatabaseError> update_profile(const Entities::DbProfile& profile);

    // Mount operations
    std::expected<std::vector<Entities::DbMount>, DatabaseError> get_mounts_by_character_id(uint64_t character_id);
    std::expected<void, DatabaseError> add_mount(const Entities::DbMount& mount);

    // Title operations
    std::expected<std::vector<Entities::DbTitle>, DatabaseError> get_titles_by_character_id(uint64_t character_id);
    std::expected<void, DatabaseError> add_title(const Entities::DbTitle& title);

    // Transaction support
    void begin_transaction();
    void commit_transaction();
    void rollback_transaction();

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace Sanctuary::Database
