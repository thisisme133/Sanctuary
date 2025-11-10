#include <Sanctuary/Gateway/GatewayConnection.h>
#include <Sanctuary/Gateway/GatewayServer.h>
#include <spdlog/spdlog.h>

namespace Sanctuary::Gateway {

GatewayConnection::GatewayConnection(
    UdpLibrary::UdpManager* manager,
    const boost::asio::ip::udp::endpoint& endpoint,
    int connect_code,
    GatewayServer* server)
    : UdpLibrary::UdpConnection(manager, endpoint, connect_code), server_(server) {

    connection_id_ = 0; // Set by server
}

void GatewayConnection::on_route_packet(std::span<const uint8_t> data) {
    if (data.size() < 2) return;

    uint16_t opcode = (data[0] << 8) | data[1];

    if (authenticated_) {
        handle_game_packet(data);
    } else {
        handle_login_packet(data);
    }
}

void GatewayConnection::on_connect_complete() {
    spdlog::info("Gateway client connection established");
}

void GatewayConnection::on_terminated() {
    spdlog::info("Gateway client connection terminated");
    if (server_) {
        server_->remove_client_connection(connection_id_);
    }
}

void GatewayConnection::handle_login_packet(std::span<const uint8_t> data) {
    // TODO: Handle login packets
    spdlog::debug("Handling login packet, size: {}", data.size());
}

void GatewayConnection::handle_game_packet(std::span<const uint8_t> data) {
    // TODO: Handle game packets
    spdlog::debug("Handling game packet, size: {}", data.size());
}

} // namespace Sanctuary::Gateway
