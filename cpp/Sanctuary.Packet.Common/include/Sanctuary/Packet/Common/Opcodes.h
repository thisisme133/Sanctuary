#pragma once

#include <cstdint>

namespace Sanctuary::Packet::Common {

enum class Opcode : uint16_t {
    // Client to Server
    LoginRequest = 0x01,
    Logout = 0x03,
    TunneledClientPacket = 0x07,
    TunneledClientWorldPacket = 0x08,
    ZoneTeleportRequest = 0x0D,
    ClientInitializationDetails = 0x15,
    ClientMetrics = 0x16,

    // Server to Client
    LoginReply = 0x02,
    TunneledClientPacketReply = 0x09,
    ZoneTeleportReply = 0x0E,
    SendZoneDetails = 0x17,

    // Character operations
    CharacterSelectInfoRequest = 0x0B,
    CharacterSelectInfoReply = 0x0C,
    CharacterCreateRequest = 0x10,
    CharacterCreateReply = 0x11,
    CharacterDeleteRequest = 0x12,
    CharacterDeleteReply = 0x13,
    CharacterLoginRequest = 0x14,
    CharacterLoginReply = 0x15,

    // Game packets
    PlayerUpdate = 0x1E,
    PlayerUpdatePosition = 0x1F,
    PlayerUpdateJump = 0x20,

    // Chat
    Chat = 0x30,
    ChatReply = 0x31,
    QuickChat = 0x32,

    // Inventory
    Inventory = 0x40,
    InventoryReply = 0x41,

    // Friends
    FriendList = 0x50,
    FriendOnline = 0x51,
    FriendOffline = 0x52,
    FriendMessage = 0x53,

    // Ignore list
    IgnoreList = 0x60,

    // Profile
    ProfileRequest = 0x70,
    ProfileReply = 0x71,

    // Server status
    ServerListRequest = 0x80,
    ServerListReply = 0x81,
    ServerStatus = 0x82,

    // World
    WorldTeleportRequest = 0x90,
    GameTimeSync = 0x91,

    // UI
    UIEvent = 0xA0,

    // Mount
    MountSpawn = 0xB0,
    MountDespawn = 0xB1,

    // Housing
    HousingEnterRequest = 0xC0,
    HousingEditMode = 0xC1,

    // Name operations
    CheckName = 0xD0,
    CheckNameReply = 0xD1,
    ChangeName = 0xD2,
    ChangeNameReply = 0xD3,

    // Title
    TitleList = 0xE0,
    TitleSelect = 0xE1,

    // Command packets
    CommandInteract = 0xF0,
    CommandSelectPlayer = 0xF1,
    CommandSetProfile = 0xF2,

    // Gateway-Login communication
    GatewayLogin = 0x1000,
    GatewayLoginReply = 0x1001,
    GatewayCharacterLogin = 0x1002,
    GatewayCharacterLogout = 0x1003,
};

} // namespace Sanctuary::Packet::Common
