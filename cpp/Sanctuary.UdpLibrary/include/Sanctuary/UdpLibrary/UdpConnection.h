#pragma once

#include "Enums.h"
#include "UdpParams.h"
#include "LogicalPacket.h"
#include <cstdint>
#include <memory>
#include <mutex>
#include <span>
#include <array>
#include <functional>
#include <boost/asio.hpp>

namespace Sanctuary::UdpLibrary {

using UdpClockStamp = int64_t;

class UdpManager;
class UdpReliableChannel;

struct UdpConnectionStatistics {
    uint64_t iterations = 0;
    uint64_t total_packets_sent = 0;
    uint64_t total_packets_received = 0;
    uint64_t total_bytes_sent = 0;
    uint64_t total_bytes_received = 0;
    uint64_t application_packets_sent = 0;
    uint64_t application_packets_received = 0;
    uint64_t crc_rejected_packets = 0;
    uint64_t order_rejected_packets = 0;
    uint64_t corrupt_packet_errors = 0;
    uint32_t average_ping_time = 0;
    uint32_t high_ping_time = 0;
    uint32_t low_ping_time = 0;
    uint32_t last_ping_time = 0;
    uint32_t master_ping_time = 0;
    int64_t master_ping_age = 0;
    float percent_sent_success = 0.0f;
    float percent_received_success = 0.0f;
    uint64_t sync_our_sent = 0;
    uint64_t sync_our_received = 0;
    uint64_t sync_their_sent = 0;
    uint64_t sync_their_received = 0;
    uint32_t reliable_average_ping = 0;
};

class UdpConnection {
public:
    // Client-side constructor
    UdpConnection(UdpManager* manager, const boost::asio::ip::udp::endpoint& endpoint, UdpClockStamp timeout);

    // Server-side constructor
    UdpConnection(UdpManager* manager, const boost::asio::ip::udp::endpoint& endpoint, int connect_code);

    virtual ~UdpConnection() = default;

    // Send methods
    bool send(UdpChannel channel, std::span<const uint8_t> data);
    bool send(UdpChannel channel, std::shared_ptr<LogicalPacket> packet);

    // Disconnect
    void disconnect(int flush_timeout = 0);

    // Packet processing
    void process_raw_packet(std::span<const uint8_t> data);
    void give_time();

    // Statistics
    void get_stats(UdpConnectionStatistics& stats);

    // Callbacks (virtual for subclasses)
    virtual void on_route_packet(std::span<const uint8_t> data) {}
    virtual void on_connect_complete() {}
    virtual void on_terminated() {}
    virtual void on_crc_reject(std::span<const uint8_t> data) {}
    virtual void on_packet_corrupt(std::span<const uint8_t> data, UdpCorruptionReason reason) {}

    // Public members
    boost::asio::ip::udp::endpoint endpoint;
    Status status{Status::Disconnected};
    DisconnectReason disconnect_reason{DisconnectReason::None};
    DisconnectReason other_side_disconnect_reason{DisconnectReason::None};
    UdpClockStamp no_data_timeout = 0;
    int connect_code = 0;

    UdpManager* udp_manager = nullptr;

private:
    // Internal structures
    struct Configuration {
        int encrypt_code = 0;
        uint8_t crc_bytes = 0;
        std::array<EncryptMethod, 2> encrypt_method{};
        int max_raw_packet_size = 512;
    };

    Configuration connection_config_;
    std::mutex guard_;
    std::mutex handler_guard_;

    UdpConnectionStatistics connection_stats_;
    UdpClockStamp connection_create_time_ = 0;
    UdpClockStamp connect_attempt_timeout_ = 0;
    UdpClockStamp last_send_time_ = 0;
    UdpClockStamp last_receive_time_ = 0;
    UdpClockStamp last_clock_sync_time_ = 0;
    UdpClockStamp data_hold_time_ = 0;
    UdpClockStamp last_port_alive_time_ = 0;

    bool flagged_port_unreachable_ = false;
    bool silent_disconnect_ = false;
    bool getting_time_ = false;

    std::vector<uint8_t> multi_buffer_data_;
    int multi_buffer_offset_ = 0;

    int ordered_count_outgoing_ = 0;
    int ordered_count_outgoing2_ = 0;
    uint16_t ordered_stamp_last_ = 0;
    uint16_t ordered_stamp_last2_ = 0;

    std::unique_ptr<uint8_t[]> encrypt_xor_buffer_;
    int encrypt_expansion_bytes_ = 0;

    uint32_t sync_time_delta_ = 0;
    uint32_t sync_stat_total_ = 0;
    uint32_t sync_stat_count_ = 0;
    uint32_t sync_stat_low_ = 0;
    uint32_t sync_stat_high_ = 0;
    uint32_t sync_stat_last_ = 0;
    uint32_t sync_stat_master_round_time_ = 0;
    UdpClockStamp sync_stat_master_fixup_time_ = 0;

    int keep_alive_delay_ = 0;

    UdpClockStamp icmp_error_retry_start_stamp_ = 0;
    UdpClockStamp port_remap_request_start_stamp_ = 0;
    UdpClockStamp disconnect_flush_stamp_ = 0;
    UdpClockStamp disconnect_flush_timeout_ = 0;

    int other_side_protocol_version_ = 0;
    std::string other_side_protocol_name_;

    std::array<std::unique_ptr<UdpReliableChannel>, 4> channels_;

    // Encryption buffers
    std::array<std::vector<uint8_t>, 2> temp_decrypt_buffer_;
    std::array<std::vector<uint8_t>, 2> temp_encrypt_buffer_;

    // Encryption functions
    using CryptFunction = std::function<int(std::span<uint8_t>, std::span<const uint8_t>)>;
    std::array<CryptFunction, 2> decrypt_functions_;
    std::array<CryptFunction, 2> encrypt_functions_;

    // Internal methods
    void internal_disconnect(int flush_timeout, DisconnectReason reason);
    void send_terminate_packet(int connect_code, DisconnectReason reason);
    bool internal_send(UdpChannel channel, std::span<const uint8_t> data1, std::span<const uint8_t> data2 = {});
    void process_cooked_packet(std::span<const uint8_t> data);
    void internal_give_time();
    void port_unreachable();
    void ping_stat_reset();
    void raw_send(std::span<const uint8_t> data);
    void physical_send(std::span<uint8_t> data, bool append_allowed);
    std::span<uint8_t> buffered_send(std::span<uint8_t> data, std::span<uint8_t> data2, bool append_allowed);
    void flush_multi_buffer();
    void schedule_time_now();
    bool is_non_encrypt_packet(UdpPacketType packet_type);
    UdpClockStamp connection_age();
    UdpClockStamp last_receive();
    int total_pending_bytes();

    // Encryption methods
    void setup_encrypt_model();
    int encrypt_none(std::span<uint8_t> dest, std::span<const uint8_t> src);
    int decrypt_none(std::span<uint8_t> dest, std::span<const uint8_t> src);
    int encrypt_xor(std::span<uint8_t> dest, std::span<const uint8_t> src);
    int decrypt_xor(std::span<uint8_t> dest, std::span<const uint8_t> src);
    int encrypt_xor_buffer(std::span<uint8_t> dest, std::span<const uint8_t> src);
    int decrypt_xor_buffer(std::span<uint8_t> dest, std::span<const uint8_t> src);

    virtual int encrypt_user_supplied(std::span<uint8_t> dest, std::span<const uint8_t> src);
    virtual int decrypt_user_supplied(std::span<uint8_t> dest, std::span<const uint8_t> src);
    virtual int encrypt_user_supplied2(std::span<uint8_t> dest, std::span<const uint8_t> src);
    virtual int decrypt_user_supplied2(std::span<uint8_t> dest, std::span<const uint8_t> src);

    // Callback helpers
    void callback_route_packet(std::span<const uint8_t> data);
    void callback_corrupt_packet(std::span<const uint8_t> data, UdpCorruptionReason reason);
};

} // namespace Sanctuary::UdpLibrary
