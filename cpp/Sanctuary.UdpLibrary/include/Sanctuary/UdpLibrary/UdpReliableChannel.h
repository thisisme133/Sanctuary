#pragma once

#include "UdpParams.h"
#include <cstdint>
#include <span>

namespace Sanctuary::UdpLibrary {

class UdpConnection;

class UdpReliableChannel {
public:
    UdpReliableChannel(int channel_num, UdpConnection* connection, const UdpReliableConfig& config);
    ~UdpReliableChannel() = default;

    void send(std::span<const uint8_t> data1, std::span<const uint8_t> data2 = {});
    void reliable_packet(std::span<const uint8_t> data);
    void ack_packet(std::span<const uint8_t> data);
    void ack_all_packet(std::span<const uint8_t> data);
    int64_t give_time();
    int total_pending_bytes() const;
    uint32_t get_average_ping() const;
    void clear_buffered_ack();

private:
    int channel_num_;
    UdpConnection* connection_;
    UdpReliableConfig config_;
};

} // namespace Sanctuary::UdpLibrary
