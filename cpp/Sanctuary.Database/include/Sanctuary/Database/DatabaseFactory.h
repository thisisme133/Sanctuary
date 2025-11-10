#pragma once

#include "DatabaseContext.h"
#include <memory>
#include <string>

namespace Sanctuary::Database {

enum class DatabaseType {
    SQLite,
    MySQL
};

class DatabaseFactory {
public:
    static std::unique_ptr<DatabaseContext> create(
        DatabaseType type,
        const std::string& connection_string);

    static std::unique_ptr<DatabaseContext> create_sqlite(const std::string& db_path);

    static std::unique_ptr<DatabaseContext> create_mysql(
        const std::string& host,
        uint16_t port,
        const std::string& database,
        const std::string& username,
        const std::string& password);
};

} // namespace Sanctuary::Database
