#pragma once

#include <string>
#include <cstdint>

namespace Sanctuary::Core::Configuration {

enum class DatabaseProvider : uint8_t {
    Sqlite,
    MySql
};

struct ServerOptions {
    int32_t port{};
    bool useCompression{false};
};

struct DatabaseOptions {
    DatabaseProvider provider{DatabaseProvider::Sqlite};
    std::string versionString;
    std::string connectionString;
};

struct GatewayServerOptions : ServerOptions {
    std::string environment;
    std::string clientVersion;
    std::string serverAddress;
    std::string loginGatewayAddress;
    std::string loginGatewayChallenge;
    bool showMemberNagScreen{false};
};

struct LoginServerOptions : ServerOptions {
    std::string cryptKey;
    int32_t loginGatewayPort{};
    std::string loginGatewayChallenge;
    bool isLocked{false};
    int32_t defaultTitleId{};
    int32_t defaultProfileId{};
    bool unlockAllItems{false};
    bool unlockAllTitles{false};
    bool unlockAllMounts{false};
    bool unlockAllProfiles{false};
};

} // namespace Sanctuary::Core::Configuration
