#pragma once

#include <cstdint>

namespace Sanctuary::UdpLibrary {

enum class UdpChannel : uint8_t {
    Unreliable,
    UnreliableUnbuffered,
    Ordered,
    OrderedUnbuffered,
    Reliable1,
    Reliable2,
    Reliable3,
    Reliable4,
    Count
};

enum class Status {
    Negotiating,
    Connected,
    Disconnected,
    DisconnectPending
};

enum class DisconnectReason : int16_t {
    None,
    IcmpError,
    Timeout,
    OtherSideTerminated,
    ManagerDeleted,
    ConnectFail,
    Application,
    UnreachableConnection,
    UnacknowledgedTimeout,
    NewConnectionAttempt,
    ConnectionRefused,
    MutualConnectError,
    ConnectingToSelf,
    ReliableOverflow,
    ApplicationReleased,
    CorruptPacket,
    OtherProtocolName
};

enum class EncryptMethod : uint8_t {
    None,
    UserSupplied,
    UserSupplied2,
    XorBuffer,
    Xor
};

enum class ManagerRole {
    Default,
    InternalServer,
    InternalClient,
    ExternalServer,
    ExternalClient,
    Lfn
};

enum class UdpPacketType : uint8_t {
    ZeroEscape,
    Connect,
    Confirm,
    Multi,
    Big,
    Terminate,
    KeepAlive,
    ClockSync,
    ClockReflect,
    Reliable1,
    Reliable2,
    Reliable3,
    Reliable4,
    Fragment1,
    Fragment2,
    Fragment3,
    Fragment4,
    Ack1,
    Ack2,
    Ack3,
    Ack4,
    AckAll1,
    AckAll2,
    AckAll3,
    AckAll4,
    Group,
    Ordered,
    Ordered2,
    PortAlive,
    UnreachableConnection,
    RequestRemap,
    Unknown,
    ServerStatus
};

enum class CallbackEventType {
    RoutePacket,
    Terminated,
    ConnectComplete,
    PacketCorrupt
};

enum class UdpCorruptionReason {
    None,
    CrcMismatch,
    PacketTooLarge,
    InvalidHeader
};

enum class ErrorCondition {
    None,
    IcmpError,
    ConnectError
};

enum class ReliablePacketMode {
    Reliable,
    Trickle
};

} // namespace Sanctuary::UdpLibrary
