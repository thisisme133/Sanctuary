#include <Sanctuary/Login/LoginServer.h>
#include <Sanctuary/Login/LoginConnection.h>
#include <Sanctuary/Database/DatabaseFactory.h>
#include <spdlog/spdlog.h>

namespace Sanctuary::Login {

LoginServer::LoginServer(
    boost::asio::io_context& io_context,
    uint16_t client_port,
    uint16_t gateway_port,
    const std::string& database_path)
    : io_context_(io_context),
      client_port_(client_port),
      gateway_port_(gateway_port) {

    initialize_database(database_path);
    setup_udp_managers();

    spdlog::info("LoginServer created - Client port: {}, Gateway port: {}",
                 client_port, gateway_port);
}

LoginServer::~LoginServer() {
    stop();
}

void LoginServer::start() {
    spdlog::info("LoginServer started");
}

void LoginServer::stop() {
    spdlog::info("LoginServer stopped");
}

void LoginServer::add_client_connection(std::shared_ptr<LoginConnection> connection) {
    client_connections_[connection->get_connection_id()] = connection;
    spdlog::info("Client connected: {}", connection->get_connection_id());
}

void LoginServer::remove_client_connection(uint64_t connection_id) {
    client_connections_.erase(connection_id);
    spdlog::info("Client disconnected: {}", connection_id);
}

void LoginServer::add_gateway_connection(std::shared_ptr<GatewayConnection> connection) {
    // TODO: Implement gateway connection management
}

void LoginServer::remove_gateway_connection(uint64_t gateway_id) {
    gateway_connections_.erase(gateway_id);
}

void LoginServer::setup_udp_managers() {
    UdpLibrary::UdpParams client_params;
    client_params.port = client_port_;
    client_params.max_connections = 1000;
    client_params.crc_bytes = 2;

    UdpLibrary::UdpParams gateway_params;
    gateway_params.port = gateway_port_;
    gateway_params.max_connections = 10;
    gateway_params.crc_bytes = 2;

    // client_udp_manager_ = std::make_unique<UdpLibrary::UdpManager>(client_params, io_context_);
    // gateway_udp_manager_ = std::make_unique<UdpLibrary::UdpManager>(gateway_params, io_context_);
}

void LoginServer::initialize_database(const std::string& database_path) {
    database_ = Database::DatabaseFactory::create_sqlite(database_path);
    spdlog::info("Database initialized: {}", database_path);
}

} // namespace Sanctuary::Login
