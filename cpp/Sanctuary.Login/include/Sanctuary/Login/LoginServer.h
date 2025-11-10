#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <boost/asio.hpp>
#include <Sanctuary/UdpLibrary/UdpManager.h>
#include <Sanctuary/UdpLibrary/UdpConnection.h>
#include <Sanctuary/Database/DatabaseContext.h>

namespace Sanctuary::Login {

class LoginConnection;
class GatewayConnection;

class LoginServer {
public:
    explicit LoginServer(
        boost::asio::io_context& io_context,
        uint16_t client_port,
        uint16_t gateway_port,
        const std::string& database_path);

    ~LoginServer();

    void start();
    void stop();

    // Connection management
    void add_client_connection(std::shared_ptr<LoginConnection> connection);
    void remove_client_connection(uint64_t connection_id);

    void add_gateway_connection(std::shared_ptr<GatewayConnection> connection);
    void remove_gateway_connection(uint64_t gateway_id);

    // Database access
    Database::DatabaseContext& get_database() { return *database_; }

    boost::asio::io_context& get_io_context() { return io_context_; }

private:
    boost::asio::io_context& io_context_;

    std::unique_ptr<UdpLibrary::UdpManager> client_udp_manager_;
    std::unique_ptr<UdpLibrary::UdpManager> gateway_udp_manager_;
    std::unique_ptr<Database::DatabaseContext> database_;

    std::unordered_map<uint64_t, std::shared_ptr<LoginConnection>> client_connections_;
    std::unordered_map<uint64_t, std::shared_ptr<GatewayConnection>> gateway_connections_;

    uint16_t client_port_;
    uint16_t gateway_port_;
    uint64_t next_connection_id_{1};

    void setup_udp_managers();
    void initialize_database(const std::string& database_path);
};

} // namespace Sanctuary::Login
