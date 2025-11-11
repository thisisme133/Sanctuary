#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <boost/asio.hpp>
#include <Sanctuary/UdpLibrary/UdpManager.h>
#include <Sanctuary/UdpLibrary/UdpConnection.h>

namespace Sanctuary::Gateway {

class GatewayConnection;
class LoginConnection;

class GatewayServer {
public:
    explicit GatewayServer(boost::asio::io_context& io_context, uint16_t port);
    ~GatewayServer();

    void start();
    void stop();
    void give_udp_time();

    // Connection management
    void add_client_connection(std::shared_ptr<GatewayConnection> connection);
    void remove_client_connection(uint64_t connection_id);

    boost::asio::io_context& get_io_context() { return io_context_; }

private:
    boost::asio::io_context& io_context_;
    std::unique_ptr<UdpLibrary::UdpManager> udp_manager_;

    std::unordered_map<uint64_t, std::shared_ptr<GatewayConnection>> client_connections_;
    std::shared_ptr<LoginConnection> login_connection_;

    uint16_t port_;
    uint64_t next_connection_id_{1};

    void setup_udp_manager();
};

} // namespace Sanctuary::Gateway
