#include <Sanctuary/UdpLibrary/UdpConnection.h>
#include <Sanctuary/UdpLibrary/LogicalPacket.h>
#include <cstdint>
#include <span>

namespace Sanctuary::UdpLibrary {

// CRC32 computation
constexpr uint32_t CRC32_POLYNOMIAL = 0xEDB88320;

uint32_t crc32(std::span<const uint8_t> data, int encrypt_code) {
    uint32_t crc = encrypt_code;

    for (auto byte : data) {
        crc ^= byte;
        for (int i = 0; i < 8; ++i) {
            crc = (crc >> 1) ^ ((crc & 1) ? CRC32_POLYNOMIAL : 0);
        }
    }

    return crc;
}

// Random number generator (simple LCG)
int random(int& seed) {
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return seed;
}

// Clock difference calculation
int64_t clock_diff(int64_t start, int64_t end) {
    return end - start;
}

// Sync stamp delta time
uint32_t sync_stamp_short_delta_time(uint16_t stamp1, uint16_t stamp2) {
    int32_t diff = static_cast<int32_t>(stamp2) - static_cast<int32_t>(stamp1);
    if (diff < 0) {
        diff += 0x10000;
    }
    return static_cast<uint32_t>(diff);
}

// Get 24-bit value
uint32_t get_value24(std::span<const uint8_t> data) {
    return (static_cast<uint32_t>(data[0]) << 16) |
           (static_cast<uint32_t>(data[1]) << 8) |
           static_cast<uint32_t>(data[2]);
}

// Put 24-bit value
void put_value24(std::span<uint8_t> data, uint32_t value) {
    data[0] = static_cast<uint8_t>((value >> 16) & 0xFF);
    data[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    data[2] = static_cast<uint8_t>(value & 0xFF);
}

// Get variable length value
int get_variable_value(std::span<const uint8_t> data, int& value) {
    if (data.empty()) {
        value = 0;
        return 0;
    }

    uint8_t first = data[0];
    if ((first & 0x80) == 0) {
        value = first;
        return 1;
    } else if (data.size() >= 2) {
        value = ((first & 0x7F) << 8) | data[1];
        return 2;
    }

    value = 0;
    return 0;
}

// Create quick logical packet
std::shared_ptr<LogicalPacket> create_quick_logical_packet(
    std::span<const uint8_t> data1,
    std::span<const uint8_t> data2) {

    auto total_size = data1.size() + data2.size();
    auto packet = std::make_shared<SimpleLogicalPacket>(total_size);
    packet->set_data(data1, data2);
    return packet;
}

} // namespace Sanctuary::UdpLibrary
