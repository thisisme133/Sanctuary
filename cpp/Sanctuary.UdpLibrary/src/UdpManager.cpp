#include <Sanctuary/UdpLibrary/UdpManager.h>
#include <spdlog/spdlog.h>
#include <chrono>

namespace Sanctuary::UdpLibrary {

UdpManager::UdpManager(const UdpParams& params, boost::asio::io_context& io_context)
    : params(params), io_context_(io_context),
      socket_(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), params.port)) {

    cached_clock_ = clock();
    random_seed_ = static_cast<int>(cached_clock_);

    spdlog::info("UdpManager created on port {}", params.port);

    start_receive();
}

UdpManager::~UdpManager() {
    close_socket();
}

std::shared_ptr<UdpConnection> UdpManager::establish_connection(
    const std::string& server_address,
    uint16_t server_port,
    UdpClockStamp timeout) {

    std::lock_guard lock(give_time_guard_);

    if (connection_list_.size() >= static_cast<size_t>(params.max_connections)) {
        return nullptr;
    }

    // Resolve address
    boost::asio::ip::udp::resolver resolver(io_context_);
    auto endpoints = resolver.resolve(
        boost::asio::ip::udp::v4(),
        server_address,
        std::to_string(server_port));

    if (endpoints.empty()) {
        return nullptr;
    }

    auto endpoint = *endpoints.begin();

    // Create connection
    auto connection = std::make_shared<UdpConnection>(this, endpoint.endpoint(), timeout);
    add_connection(connection);

    return connection;
}

void UdpManager::add_connection(std::shared_ptr<UdpConnection> connection) {
    std::lock_guard lock(connection_guard_);

    connection_list_.push_back(connection);

    auto endpoint_hash = address_hash_value(connection->endpoint);
    address_hash_table_[endpoint_hash] = connection;

    connect_code_hash_table_[connection->connect_code] = connection;
}

void UdpManager::remove_connection(UdpConnection* connection) {
    std::lock_guard lock(connection_guard_);

    auto endpoint_hash = address_hash_value(connection->endpoint);
    address_hash_table_.erase(endpoint_hash);

    connect_code_hash_table_.erase(connection->connect_code);

    connection_list_.erase(
        std::remove_if(connection_list_.begin(), connection_list_.end(),
            [connection](const auto& conn) { return conn.get() == connection; }),
        connection_list_.end());
}

void UdpManager::keep_until_disconnected(UdpConnection* connection) {
    std::lock_guard lock(disconnect_pending_guard_);
    disconnect_pending_list_.push_back(connection);
}

bool UdpManager::give_time(int max_polling_time, bool give_connections_time) {
    std::lock_guard lock(give_time_guard_);

    cached_clock_ = clock();

    manager_stats_.iterations++;

    // Give time to all connections
    if (give_connections_time) {
        for (auto& conn : connection_list_) {
            if (conn) {
                conn->give_time();
            }
        }

        process_disconnect_pending();
    }

    return false;
}

UdpClockStamp UdpManager::clock() {
    std::lock_guard lock(clock_guard_);
    auto now = std::chrono::steady_clock::now();
    cached_clock_ = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    return cached_clock_;
}

UdpClockStamp UdpManager::cached_clock_elapsed(UdpClockStamp start) const {
    return cached_clock_ - start;
}

uint32_t UdpManager::local_sync_stamp_long() {
    return static_cast<uint32_t>(clock());
}

uint16_t UdpManager::local_sync_stamp_short() {
    return static_cast<uint16_t>(clock());
}

std::shared_ptr<LogicalPacket> UdpManager::create_packet(
    std::span<const uint8_t> data1,
    std::span<const uint8_t> data2) {

    auto total_size = data1.size() + data2.size();

    if (params.pooled_packet_max > 0 && total_size <= static_cast<size_t>(params.pooled_packet_size)) {
        auto packet = std::make_shared<PooledLogicalPacket>(params.pooled_packet_size);
        packet->set_data(data1, data2);
        return packet;
    }

    auto packet = std::make_shared<SimpleLogicalPacket>(total_size);
    packet->set_data(data1, data2);
    return packet;
}

void UdpManager::actual_send(std::span<const uint8_t> data, const boost::asio::ip::udp::endpoint& endpoint) {
    std::lock_guard lock(stats_guard_);
    last_send_time_ = cached_clock_;
    manager_stats_.bytes_sent += data.size();
    manager_stats_.packets_sent++;

    socket_.async_send_to(
        boost::asio::buffer(data.data(), data.size()),
        endpoint,
        [](const boost::system::error_code& error, std::size_t) {
            if (error) {
                spdlog::warn("UDP send error: {}", error.message());
            }
        });
}

void UdpManager::send_port_alive(const boost::asio::ip::udp::endpoint& endpoint) {
    uint8_t buf[2] = {0, static_cast<uint8_t>(UdpPacketType::PortAlive)};
    socket_.async_send_to(
        boost::asio::buffer(buf, 2),
        endpoint,
        [](const boost::system::error_code&, std::size_t) {});
}

void UdpManager::get_stats(UdpManagerStatistics& stats) {
    std::lock_guard lock(stats_guard_);
    stats = manager_stats_;
    stats.connection_count = connection_list_.size();
    stats.elapsed_time = cached_clock_elapsed(manager_stats_reset_time_);
}

void UdpManager::reset_stats() {
    std::lock_guard lock(stats_guard_);
    manager_stats_reset_time_ = cached_clock_;
    manager_stats_.reset();
}

int UdpManager::random() {
    int result = (random_seed_ * 1103515245 + 12345) & 0x7FFFFFFF;
    random_seed_ = result;
    return result;
}

void UdpManager::start_receive() {
    socket_.async_receive_from(
        boost::asio::buffer(receive_buffer_),
        remote_endpoint_,
        [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
            handle_receive(error, bytes_transferred);
        });
}

void UdpManager::handle_receive(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error && bytes_transferred > 0) {
        std::lock_guard lock(stats_guard_);
        last_receive_time_ = cached_clock_;
        manager_stats_.bytes_received += bytes_transferred;
        manager_stats_.packets_received++;

        std::span<const uint8_t> data(receive_buffer_.data(), bytes_transferred);
        process_raw_packet(remote_endpoint_, data);
    }

    start_receive();
}

void UdpManager::process_raw_packet(const boost::asio::ip::udp::endpoint& endpoint, std::span<const uint8_t> data) {
    auto connection = address_get_connection(endpoint);

    if (connection) {
        connection->process_raw_packet(data);
    } else {
        // Handle new connection request
        // TODO: Parse connect packet and create new connection if appropriate
    }
}

void UdpManager::process_disconnect_pending() {
    std::lock_guard lock(disconnect_pending_guard_);

    disconnect_pending_list_.erase(
        std::remove_if(disconnect_pending_list_.begin(), disconnect_pending_list_.end(),
            [](UdpConnection* conn) { return conn->status == Status::Disconnected; }),
        disconnect_pending_list_.end());
}

UdpConnection* UdpManager::address_get_connection(const boost::asio::ip::udp::endpoint& endpoint) {
    std::lock_guard lock(connection_guard_);

    auto hash = address_hash_value(endpoint);
    auto it = address_hash_table_.find(hash);

    if (it != address_hash_table_.end()) {
        return it->second.get();
    }

    return nullptr;
}

UdpConnection* UdpManager::connect_code_get_connection(int connect_code) {
    std::lock_guard lock(connection_guard_);

    auto it = connect_code_hash_table_.find(connect_code);
    if (it != connect_code_hash_table_.end()) {
        return it->second.get();
    }

    return nullptr;
}

size_t UdpManager::address_hash_value(const boost::asio::ip::udp::endpoint& endpoint) {
    auto addr = endpoint.address().to_v4().to_bytes();
    auto port = endpoint.port();

    size_t hash = 0;
    for (auto byte : addr) {
        hash = hash * 31 + byte;
    }
    hash = hash * 31 + port;

    return hash;
}

void UdpManager::create_and_bind_socket(uint16_t use_port) {
    // Already handled in constructor
}

void UdpManager::close_socket() {
    if (socket_.is_open()) {
        boost::system::error_code ec;
        socket_.close(ec);
    }
}

// Callback implementations (can be overridden)
void UdpManager::callback_route_packet(UdpConnection* con, std::span<const uint8_t> data) {
    con->on_route_packet(data);
}

void UdpManager::callback_crc_reject(UdpConnection* con, std::span<const uint8_t> data) {
    con->on_crc_reject(data);
}

void UdpManager::callback_packet_corrupt(UdpConnection* con, std::span<const uint8_t> data, UdpCorruptionReason reason) {
    con->on_packet_corrupt(data, reason);
}

void UdpManager::callback_connect_complete(UdpConnection* con) {
    con->on_connect_complete();
}

void UdpManager::callback_terminated(UdpConnection* con) {
    con->on_terminated();
}

bool UdpManager::on_connect_request(UdpConnection* con) {
    return false; // Override in subclass
}

void UdpManager::on_server_status_request(const boost::asio::ip::udp::endpoint& endpoint) {
    // Override in subclass if needed
}

// Stats increment helpers
void UdpManager::increment_crc_rejected_packets() {
    std::lock_guard lock(stats_guard_);
    manager_stats_.crc_rejected_packets++;
}

void UdpManager::increment_order_rejected_packets() {
    std::lock_guard lock(stats_guard_);
    manager_stats_.order_rejected_packets++;
}

void UdpManager::increment_duplicate_packets_received() {
    std::lock_guard lock(stats_guard_);
    manager_stats_.duplicate_packets_received++;
}

void UdpManager::increment_resent_packets_accelerated() {
    std::lock_guard lock(stats_guard_);
    manager_stats_.resent_packets_accelerated++;
}

void UdpManager::increment_resent_packets_timed_out() {
    std::lock_guard lock(stats_guard_);
    manager_stats_.resent_packets_timed_out++;
}

void UdpManager::increment_application_packets_sent() {
    std::lock_guard lock(stats_guard_);
    manager_stats_.application_packets_sent++;
}

void UdpManager::increment_application_packets_received() {
    std::lock_guard lock(stats_guard_);
    manager_stats_.application_packets_received++;
}

void UdpManager::increment_corrupt_packet_errors() {
    std::lock_guard lock(stats_guard_);
    manager_stats_.corrupt_packet_errors++;
}

} // namespace Sanctuary::UdpLibrary
