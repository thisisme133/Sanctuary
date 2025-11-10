#pragma once

#include "Enums.h"
#include "UdpParams.h"
#include "UdpConnection.h"
#include "LogicalPacket.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <boost/asio.hpp>

namespace Sanctuary::UdpLibrary {

struct UdpManagerStatistics {
    uint64_t iterations = 0;
    uint64_t bytes_sent = 0;
    uint64_t bytes_received = 0;
    uint64_t packets_sent = 0;
    uint64_t packets_received = 0;
    uint64_t application_packets_sent = 0;
    uint64_t application_packets_received = 0;
    uint64_t crc_rejected_packets = 0;
    uint64_t order_rejected_packets = 0;
    uint64_t duplicate_packets_received = 0;
    uint64_t resent_packets_accelerated = 0;
    uint64_t resent_packets_timed_out = 0;
    uint64_t corrupt_packet_errors = 0;
    uint64_t socket_overflow_errors = 0;
    uint64_t max_polling_time_exceeded = 0;
    uint64_t max_delivery_time_exceeded = 0;
    uint64_t priority_queue_processed = 0;
    uint64_t priority_queue_possible = 0;
    int connection_count = 0;
    int disconnect_pending_count = 0;
    int event_list_count = 0;
    int event_list_bytes = 0;
    int64_t elapsed_time = 0;

    void reset() {
        *this = {};
    }
};

class UdpManager {
public:
    explicit UdpManager(const UdpParams& params, boost::asio::io_context& io_context);
    virtual ~UdpManager();

    // Connection management
    std::shared_ptr<UdpConnection> establish_connection(
        const std::string& server_address,
        uint16_t server_port,
        UdpClockStamp timeout = 0);

    void add_connection(std::shared_ptr<UdpConnection> connection);
    void remove_connection(UdpConnection* connection);
    void keep_until_disconnected(UdpConnection* connection);

    // Time management
    bool give_time(int max_polling_time = 500, bool give_connections_time = true);
    UdpClockStamp clock();
    UdpClockStamp cached_clock() const { return cached_clock_; }
    UdpClockStamp cached_clock_elapsed(UdpClockStamp start) const;
    uint32_t local_sync_stamp_long();
    uint16_t local_sync_stamp_short();

    // Packet creation
    std::shared_ptr<LogicalPacket> create_packet(
        std::span<const uint8_t> data1,
        std::span<const uint8_t> data2 = {});

    // Sending
    void actual_send(std::span<const uint8_t> data, const boost::asio::ip::udp::endpoint& endpoint);
    void send_port_alive(const boost::asio::ip::udp::endpoint& endpoint);

    // Statistics
    void get_stats(UdpManagerStatistics& stats);
    void reset_stats();

    // Statistics increment helpers
    void increment_crc_rejected_packets();
    void increment_order_rejected_packets();
    void increment_duplicate_packets_received();
    void increment_resent_packets_accelerated();
    void increment_resent_packets_timed_out();
    void increment_application_packets_sent();
    void increment_application_packets_received();
    void increment_corrupt_packet_errors();

    // Callbacks (can be overridden)
    virtual void callback_route_packet(UdpConnection* con, std::span<const uint8_t> data);
    virtual void callback_crc_reject(UdpConnection* con, std::span<const uint8_t> data);
    virtual void callback_packet_corrupt(UdpConnection* con, std::span<const uint8_t> data, UdpCorruptionReason reason);
    virtual void callback_connect_complete(UdpConnection* con);
    virtual void callback_terminated(UdpConnection* con);
    virtual bool on_connect_request(UdpConnection* con);
    virtual void on_server_status_request(const boost::asio::ip::udp::endpoint& endpoint);

    // Random number generation
    int random();

    // Public members
    UdpParams params;
    UdpClockStamp processing_induced_lag = 0;

private:
    boost::asio::io_context& io_context_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    std::array<uint8_t, 2048> receive_buffer_;

    UdpClockStamp cached_clock_ = 0;
    UdpClockStamp last_receive_time_ = 0;
    UdpClockStamp last_send_time_ = 0;
    UdpClockStamp last_empty_socket_buffer_stamp_ = 0;
    UdpClockStamp manager_stats_reset_time_ = 0;
    UdpClockStamp minimum_scheduled_stamp_ = 0;

    int random_seed_ = 0;

    std::mutex clock_guard_;
    std::mutex connection_guard_;
    std::mutex stats_guard_;
    std::mutex give_time_guard_;
    std::mutex disconnect_pending_guard_;

    std::vector<std::shared_ptr<UdpConnection>> connection_list_;
    std::vector<UdpConnection*> disconnect_pending_list_;
    std::unordered_map<size_t, std::shared_ptr<UdpConnection>> address_hash_table_;
    std::unordered_map<int, std::shared_ptr<UdpConnection>> connect_code_hash_table_;

    UdpManagerStatistics manager_stats_;
    ErrorCondition error_condition_ = ErrorCondition::None;

    // Internal methods
    void start_receive();
    void handle_receive(const boost::system::error_code& error, size_t bytes_transferred);
    void process_raw_packet(const boost::asio::ip::udp::endpoint& endpoint, std::span<const uint8_t> data);
    void process_disconnect_pending();
    UdpConnection* address_get_connection(const boost::asio::ip::udp::endpoint& endpoint);
    UdpConnection* connect_code_get_connection(int connect_code);
    size_t address_hash_value(const boost::asio::ip::udp::endpoint& endpoint);
    void create_and_bind_socket(uint16_t use_port);
    void close_socket();
};

} // namespace Sanctuary::UdpLibrary
