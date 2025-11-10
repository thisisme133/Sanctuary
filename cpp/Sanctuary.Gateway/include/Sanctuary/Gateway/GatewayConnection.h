#pragma once

#include <memory>
#include <cstdint>
#include <Sanctuary/UdpLibrary/UdpConnection.h>

namespace Sanctuary::Gateway {

class GatewayServer;

class GatewayConnection : public UdpLibrary::UdpConnection {
public:
    GatewayConnection(
        UdpLibrary::UdpManager* manager,
        const boost::asio::ip::udp::endpoint& endpoint,
        int connect_code,
        GatewayServer* server);

    // Override callbacks
    void on_route_packet(std::span<const uint8_t> data) override;
    void on_connect_complete() override;
    void on_terminated() override;

    // Client info
    uint64_t get_connection_id() const { return connection_id_; }
    uint64_t get_session_id() const { return session_id_; }
    uint64_t get_character_id() const { return character_id_; }

    void set_session_id(uint64_t id) { session_id_ = id; }
    void set_character_id(uint64_t id) { character_id_ = id; }

    bool is_authenticated() const { return authenticated_; }
    void set_authenticated(bool auth) { authenticated_ = auth; }

private:
    GatewayServer* server_;
    uint64_t connection_id_{0};
    uint64_t session_id_{0};
    uint64_t character_id_{0};
    bool authenticated_{false};

    void handle_login_packet(std::span<const uint8_t> data);
    void handle_game_packet(std::span<const uint8_t> data);
};

} // namespace Sanctuary::Gateway
