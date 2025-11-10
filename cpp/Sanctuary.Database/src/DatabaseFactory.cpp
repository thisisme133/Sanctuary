#include <Sanctuary/Database/DatabaseFactory.h>
#include <stdexcept>
#include <format>

namespace Sanctuary::Database {

std::unique_ptr<DatabaseContext> DatabaseFactory::create(
    DatabaseType type,
    const std::string& connection_string) {

    switch (type) {
        case DatabaseType::SQLite:
            return std::make_unique<DatabaseContext>(connection_string);

        case DatabaseType::MySQL:
            // TODO: Implement MySQL connection
            throw std::runtime_error("MySQL not yet implemented");

        default:
            throw std::invalid_argument("Unknown database type");
    }
}

std::unique_ptr<DatabaseContext> DatabaseFactory::create_sqlite(const std::string& db_path) {
    return std::make_unique<DatabaseContext>(db_path);
}

std::unique_ptr<DatabaseContext> DatabaseFactory::create_mysql(
    const std::string& host,
    uint16_t port,
    const std::string& database,
    const std::string& username,
    const std::string& password) {

    auto connection_string = std::format(
        "Server={};Port={};Database={};Uid={};Pwd={};",
        host, port, database, username, password);

    return create(DatabaseType::MySQL, connection_string);
}

} // namespace Sanctuary::Database
