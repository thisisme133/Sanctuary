#pragma once

#include <memory>
#include <cstdint>
#include <Sanctuary/UdpLibrary/UdpConnection.h>

namespace Sanctuary::Login {

class LoginServer;

class LoginConnection : public UdpLibrary::UdpConnection {
public:
    LoginConnection(
        UdpLibrary::UdpManager* manager,
        const boost::asio::ip::udp::endpoint& endpoint,
        int connect_code,
        LoginServer* server);

    // Override callbacks
    void on_route_packet(std::span<const uint8_t> data) override;
    void on_connect_complete() override;
    void on_terminated() override;

    // Client info
    uint64_t get_connection_id() const { return connection_id_; }
    uint64_t get_session_id() const { return session_id_; }
    uint64_t get_user_id() const { return user_id_; }

    void set_session_id(uint64_t id) { session_id_ = id; }
    void set_user_id(uint64_t id) { user_id_ = id; }

    bool is_authenticated() const { return authenticated_; }
    void set_authenticated(bool auth) { authenticated_ = auth; }

private:
    LoginServer* server_;
    uint64_t connection_id_{0};
    uint64_t session_id_{0};
    uint64_t user_id_{0};
    bool authenticated_{false};

    void handle_login_request(std::span<const uint8_t> data);
    void handle_character_request(std::span<const uint8_t> data);
};

} // namespace Sanctuary::Login
