#include <Sanctuary/Login/LoginConnection.h>
#include <Sanctuary/Login/LoginServer.h>
#include <spdlog/spdlog.h>

namespace Sanctuary::Login {

LoginConnection::LoginConnection(
    UdpLibrary::UdpManager* manager,
    const boost::asio::ip::udp::endpoint& endpoint,
    int connect_code,
    LoginServer* server)
    : UdpLibrary::UdpConnection(manager, endpoint, connect_code), server_(server) {

    connection_id_ = 0; // Set by server
}

void LoginConnection::on_route_packet(std::span<const uint8_t> data) {
    if (data.size() < 2) return;

    uint16_t opcode = (data[0] << 8) | data[1];

    if (authenticated_) {
        handle_character_request(data);
    } else {
        handle_login_request(data);
    }
}

void LoginConnection::on_connect_complete() {
    spdlog::info("Login client connection established");
}

void LoginConnection::on_terminated() {
    spdlog::info("Login client connection terminated");
    if (server_) {
        server_->remove_client_connection(connection_id_);
    }
}

void LoginConnection::handle_login_request(std::span<const uint8_t> data) {
    // TODO: Parse and handle login request
    spdlog::debug("Handling login request, size: {}", data.size());
}

void LoginConnection::handle_character_request(std::span<const uint8_t> data) {
    // TODO: Parse and handle character request
    spdlog::debug("Handling character request, size: {}", data.size());
}

} // namespace Sanctuary::Login
