#include <Sanctuary/UdpLibrary/UdpConnection.h>
#include <Sanctuary/UdpLibrary/UdpManager.h>
#include <spdlog/spdlog.h>

namespace Sanctuary::UdpLibrary {

// Client-side constructor
UdpConnection::UdpConnection(
    UdpManager* manager,
    const boost::asio::ip::udp::endpoint& ep,
    UdpClockStamp timeout)
    : udp_manager(manager), endpoint(ep), connect_attempt_timeout_(timeout) {

    status = Status::Negotiating;
    connect_code = manager->random();
    connection_create_time_ = manager->cached_clock();

    multi_buffer_data_.resize(manager->params.maxRawPacketSize);

    // Initialize encryption buffers
    for (auto& buf : temp_decrypt_buffer_) {
        buf.resize(2048); // HardMaxRawPacketSize
    }
    for (auto& buf : temp_encrypt_buffer_) {
        buf.resize(2048 + sizeof(int));
    }

    // Setup default encryption functions
    setup_encrypt_model();
}

// Server-side constructor
UdpConnection::UdpConnection(
    UdpManager* manager,
    const boost::asio::ip::udp::endpoint& ep,
    int code)
    : udp_manager(manager), endpoint(ep), connect_code(code) {

    status = Status::Connected;
    connection_create_time_ = manager->cached_clock();
    last_receive_time_ = manager->cached_clock();

    connection_config_.encrypt_code = manager->random();
    connection_config_.crc_bytes = manager->params.crcBytes;
    connection_config_.max_raw_packet_size = manager->params.maxRawPacketSize;

    multi_buffer_data_.resize(manager->params.maxRawPacketSize);

    for (auto& buf : temp_decrypt_buffer_) {
        buf.resize(2048);
    }
    for (auto& buf : temp_encrypt_buffer_) {
        buf.resize(2048 + sizeof(int));
    }

    setup_encrypt_model();
}

bool UdpConnection::send(UdpChannel channel, std::span<const uint8_t> data) {
    std::lock_guard lock(guard_);

    if (status != Status::Connected) {
        return false;
    }

    if (data.empty()) {
        return false;
    }

    return internal_send(channel, data);
}

bool UdpConnection::send(UdpChannel channel, std::shared_ptr<LogicalPacket> packet) {
    if (!packet) return false;
    auto data = packet->get_data_ptr();
    return send(channel, std::span<const uint8_t>{data.data(), data.size()});
}

void UdpConnection::disconnect(int flush_timeout) {
    std::lock_guard lock(guard_);
    internal_disconnect(flush_timeout, DisconnectReason::Application);
}

void UdpConnection::process_raw_packet(std::span<const uint8_t> data) {
    std::lock_guard lock(guard_);
    last_receive_time_ = udp_manager->cached_clock();
    connection_stats_.total_packets_received++;
    connection_stats_.total_bytes_received += data.size();

    // TODO: Full packet processing implementation
    process_cooked_packet(data);
}

void UdpConnection::give_time() {
    std::lock_guard lock(guard_);
    getting_time_ = true;
    internal_give_time();
    getting_time_ = false;
}

void UdpConnection::get_stats(UdpConnectionStatistics& stats) {
    std::lock_guard lock(guard_);
    stats = connection_stats_;
}

// Internal methods (stub implementations)
void UdpConnection::internal_disconnect(int flush_timeout, DisconnectReason reason) {
    if (disconnect_reason == DisconnectReason::None) {
        disconnect_reason = reason;
    }
    status = Status::Disconnected;
    if (udp_manager) {
        udp_manager->remove_connection(this);
    }
}

void UdpConnection::send_terminate_packet(int code, DisconnectReason reason) {
    // TODO: Implement
}

bool UdpConnection::internal_send(UdpChannel channel, std::span<const uint8_t> data1, std::span<const uint8_t> data2) {
    // TODO: Full implementation
    return true;
}

void UdpConnection::process_cooked_packet(std::span<const uint8_t> data) {
    // Route to application
    callback_route_packet(data);
}

void UdpConnection::internal_give_time() {
    connection_stats_.iterations++;
    // TODO: Full implementation
}

void UdpConnection::port_unreachable() {
    internal_disconnect(0, DisconnectReason::IcmpError);
}

void UdpConnection::ping_stat_reset() {
    sync_stat_count_ = 0;
    sync_stat_total_ = 0;
    sync_stat_low_ = 0;
    sync_stat_high_ = 0;
}

void UdpConnection::raw_send(std::span<const uint8_t> data) {
    if (udp_manager) {
        udp_manager->actual_send(data, endpoint);
        last_send_time_ = udp_manager->cached_clock();
    }
}

void UdpConnection::physical_send(std::span<uint8_t> data, bool append_allowed) {
    raw_send(data);
}

std::span<uint8_t> UdpConnection::buffered_send(std::span<uint8_t> data, std::span<uint8_t> data2, bool append_allowed) {
    physical_send(data, append_allowed);
    return {};
}

void UdpConnection::flush_multi_buffer() {
    // TODO: Implement
}

void UdpConnection::schedule_time_now() {
    // TODO: Implement with priority queue
}

bool UdpConnection::is_non_encrypt_packet(UdpPacketType packet_type) {
    return packet_type == UdpPacketType::Connect ||
           packet_type == UdpPacketType::Confirm ||
           packet_type == UdpPacketType::UnreachableConnection;
}

UdpClockStamp UdpConnection::connection_age() {
    return udp_manager->cached_clock_elapsed(connection_create_time_);
}

UdpClockStamp UdpConnection::last_receive() {
    return udp_manager->cached_clock_elapsed(last_receive_time_);
}

int UdpConnection::total_pending_bytes() {
    return 0; // TODO: Calculate from reliable channels
}

void UdpConnection::setup_encrypt_model() {
    encrypt_expansion_bytes_ = 0;

    for (size_t i = 0; i < connection_config_.encrypt_method.size(); ++i) {
        switch (connection_config_.encrypt_method[i]) {
            case EncryptMethod::None:
                decrypt_functions_[i] = [this](auto d, auto s) { return decrypt_none(d, s); };
                encrypt_functions_[i] = [this](auto d, auto s) { return encrypt_none(d, s); };
                break;
            case EncryptMethod::Xor:
                decrypt_functions_[i] = [this](auto d, auto s) { return decrypt_xor(d, s); };
                encrypt_functions_[i] = [this](auto d, auto s) { return encrypt_xor(d, s); };
                break;
            case EncryptMethod::XorBuffer:
                decrypt_functions_[i] = [this](auto d, auto s) { return decrypt_xor_buffer(d, s); };
                encrypt_functions_[i] = [this](auto d, auto s) { return encrypt_xor_buffer(d, s); };
                break;
            default:
                decrypt_functions_[i] = [this](auto d, auto s) { return decrypt_none(d, s); };
                encrypt_functions_[i] = [this](auto d, auto s) { return encrypt_none(d, s); };
        }
    }
}

int UdpConnection::encrypt_none(std::span<uint8_t> dest, std::span<const uint8_t> src) {
    std::copy(src.begin(), src.end(), dest.begin());
    return src.size();
}

int UdpConnection::decrypt_none(std::span<uint8_t> dest, std::span<const uint8_t> src) {
    std::copy(src.begin(), src.end(), dest.begin());
    return src.size();
}

int UdpConnection::encrypt_xor(std::span<uint8_t> dest, std::span<const uint8_t> src) {
    uint32_t prev = connection_config_.encrypt_code;
    for (size_t i = 0; i < src.size(); ++i) {
        uint32_t value = src[i] ^ prev;
        dest[i] = static_cast<uint8_t>(value);
        prev = value;
    }
    return src.size();
}

int UdpConnection::decrypt_xor(std::span<uint8_t> dest, std::span<const uint8_t> src) {
    uint32_t prev = connection_config_.encrypt_code;
    for (size_t i = 0; i < src.size(); ++i) {
        dest[i] = src[i] ^ prev;
        prev = src[i];
    }
    return src.size();
}

int UdpConnection::encrypt_xor_buffer(std::span<uint8_t> dest, std::span<const uint8_t> src) {
    if (!encrypt_xor_buffer_) return -1;
    // TODO: Implement XOR buffer encryption
    return encrypt_xor(dest, src);
}

int UdpConnection::decrypt_xor_buffer(std::span<uint8_t> dest, std::span<const uint8_t> src) {
    if (!encrypt_xor_buffer_) return -1;
    // TODO: Implement XOR buffer decryption
    return decrypt_xor(dest, src);
}

int UdpConnection::encrypt_user_supplied(std::span<uint8_t> dest, std::span<const uint8_t> src) {
    return encrypt_none(dest, src);
}

int UdpConnection::decrypt_user_supplied(std::span<uint8_t> dest, std::span<const uint8_t> src) {
    return decrypt_none(dest, src);
}

int UdpConnection::encrypt_user_supplied2(std::span<uint8_t> dest, std::span<const uint8_t> src) {
    return encrypt_none(dest, src);
}

int UdpConnection::decrypt_user_supplied2(std::span<uint8_t> dest, std::span<const uint8_t> src) {
    return decrypt_none(dest, src);
}

void UdpConnection::callback_route_packet(std::span<const uint8_t> data) {
    if (status != Status::Connected) return;

    udp_manager->increment_application_packets_received();
    connection_stats_.application_packets_received++;

    on_route_packet(data);
}

void UdpConnection::callback_corrupt_packet(std::span<const uint8_t> data, UdpCorruptionReason reason) {
    connection_stats_.corrupt_packet_errors++;
    on_packet_corrupt(data, reason);
}

} // namespace Sanctuary::UdpLibrary

// Destructor must be defined in .cpp to allow unique_ptr of incomplete type (UdpReliableChannel)
UdpConnection::~UdpConnection() = default;
