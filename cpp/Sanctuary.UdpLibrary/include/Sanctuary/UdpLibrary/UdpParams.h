#pragma once

#include "Enums.h"
#include <string>
#include <array>
#include <cstdint>

namespace Sanctuary::UdpLibrary {

constexpr size_t RELIABLE_CHANNEL_COUNT = 4;
constexpr size_t ENCRYPT_PASSES = 2;

struct UdpReliableConfig {
    int32_t maxInstandingPackets{400};
    int32_t maxOutstandingBytes{200 * 1024};
    int32_t maxOutstandingPackets{400};
    bool outOfOrder{false};
    bool coalesce{true};
    bool ackDeduping{true};
    int32_t fragmentSize{0};
    int32_t resendDelayAdjust{300};
    int32_t resendDelayPercent{125};
    int32_t resendDelayCap{8000};
    int32_t toleranceLossCount{0};
    int32_t congestionWindowMinimum{0};
    int32_t congestionWindowMaximum{8 * 1024};
    int32_t trickleRate{0};
    int32_t trickleSize{0};
};

struct UdpParams {
    int32_t maxConnections{10};
    int32_t port{0};
    int32_t portRange{0};
    int32_t outgoingBufferSize{64 * 1024};
    int32_t incomingBufferSize{64 * 1024};
    int32_t packetHistoryMax{4};
    int32_t keepAliveDelay{0};
    int32_t portAliveDelay{0};
    bool replyUnreachableConnection{true};
    bool allowPortRemapping{true};
    bool allowAddressRemapping{false};
    int32_t icmpErrorRetryPeriod{5000};
    int32_t noDataTimeout{0};
    int32_t oldestUnacknowledgedTimeout{120000};
    int32_t reliableOverflowBytes{0};
    int32_t maxDataHoldTime{50};
    int32_t maxDataHoldSize{-1};
    int32_t maxRawPacketSize{512};
    int32_t hashTableSize{100};
    bool avoidPriorityQueue{false};
    int32_t clockSyncDelay{0};
    int32_t lingerDelay{10};
    int32_t pooledPacketMax{1000};
    int32_t pooledPacketInitial{0};
    int32_t pooledPacketSize{-1};
    int32_t callbackEventPoolMax{5000};
    bool processIcmpErrors{true};
    bool processIcmpErrorsDuringNegotiating{false};
    int32_t connectAttemptDelay{1000};
    int32_t threadSleepTime{20};
    int32_t incomingLogicalPacketMax{20 * 1024 * 1024};
    std::string protocolName;
    int32_t userSuppliedEncryptExpansionBytes{0};
    int32_t userSuppliedEncryptExpansionBytes2{0};
    std::string bindIpAddress;
    bool eventQueuing{false};
    uint8_t crcBytes{0};
    std::array<EncryptMethod, ENCRYPT_PASSES> encryptMethod{EncryptMethod::None, EncryptMethod::None};
    std::array<UdpReliableConfig, RELIABLE_CHANNEL_COUNT> reliable;

    explicit UdpParams(ManagerRole role = ManagerRole::Default);
};

} // namespace Sanctuary::UdpLibrary
