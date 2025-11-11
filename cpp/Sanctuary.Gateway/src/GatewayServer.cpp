#include <Sanctuary/Gateway/GatewayServer.h>
#include <Sanctuary/Gateway/GatewayConnection.h>
#include <spdlog/spdlog.h>

namespace Sanctuary::Gateway {

GatewayServer::GatewayServer(boost::asio::io_context& io_context, uint16_t port)
    : io_context_(io_context), port_(port) {

    setup_udp_manager();
    spdlog::info("GatewayServer created on port {}", port);
}

GatewayServer::~GatewayServer() {
    stop();
}

void GatewayServer::start() {
    spdlog::info("GatewayServer started");
}

void GatewayServer::stop() {
    spdlog::info("GatewayServer stopped");
}

void GatewayServer::add_client_connection(std::shared_ptr<GatewayConnection> connection) {
    client_connections_[connection->get_connection_id()] = connection;
    spdlog::info("Client connected: {}", connection->get_connection_id());
}

void GatewayServer::remove_client_connection(uint64_t connection_id) {
    client_connections_.erase(connection_id);
    spdlog::info("Client disconnected: {}", connection_id);
}

void GatewayServer::setup_udp_manager() {
    UdpLibrary::UdpParams params;
    params.port = port_;
    params.maxConnections = 1000;
    params.crcBytes = 2;

    // udp_manager_ = std::make_unique<UdpLibrary::UdpManager>(params, io_context_);
}

} // namespace Sanctuary::Gateway
