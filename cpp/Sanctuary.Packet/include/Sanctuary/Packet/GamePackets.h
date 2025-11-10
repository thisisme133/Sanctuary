#pragma once

#include "PacketSerializer.h"
#include <Sanctuary/Packet/Common/Opcodes.h>
#include <cstdint>
#include <string>
#include <vector>

namespace Sanctuary::Packet {

struct Position {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    float rotation{0.0f};

    void serialize(Core::PacketWriter& writer) const {
        writer.write(x);
        writer.write(y);
        writer.write(z);
        writer.write(rotation);
    }

    bool deserialize(Core::PacketReader& reader) {
        return reader.read(x) &&
               reader.read(y) &&
               reader.read(z) &&
               reader.read(rotation);
    }
};

struct PlayerUpdatePosition : public BasePacket {
    uint64_t character_id{0};
    Position position;
    uint32_t sequence{0};

    void serialize(Core::PacketWriter& writer) const override {
        writer.write(character_id);
        position.serialize(writer);
        writer.write(sequence);
    }

    bool deserialize(Core::PacketReader& reader) override {
        return reader.read(character_id) &&
               position.deserialize(reader) &&
               reader.read(sequence);
    }

    uint16_t get_opcode() const override {
        return static_cast<uint16_t>(Common::Opcode::PlayerUpdatePosition);
    }
};

struct ChatMessage : public BasePacket {
    uint64_t sender_id{0};
    std::string sender_name;
    std::string message;
    uint32_t channel{0};  // 0=say, 1=tell, 2=guild, etc.

    void serialize(Core::PacketWriter& writer) const override {
        writer.write(sender_id);
        writer.write_string(sender_name);
        writer.write_string(message);
        writer.write(channel);
    }

    bool deserialize(Core::PacketReader& reader) override {
        return reader.read(sender_id) &&
               reader.read_string(sender_name) &&
               reader.read_string(message) &&
               reader.read(channel);
    }

    uint16_t get_opcode() const override {
        return static_cast<uint16_t>(Common::Opcode::Chat);
    }
};

struct ItemData {
    uint64_t item_id{0};
    uint32_t template_id{0};
    uint32_t stack_size{1};
    uint32_t slot{0};
    bool is_equipped{false};

    void serialize(Core::PacketWriter& writer) const {
        writer.write(item_id);
        writer.write(template_id);
        writer.write(stack_size);
        writer.write(slot);
        writer.write(is_equipped);
    }

    bool deserialize(Core::PacketReader& reader) {
        return reader.read(item_id) &&
               reader.read(template_id) &&
               reader.read(stack_size) &&
               reader.read(slot) &&
               reader.read(is_equipped);
    }
};

struct InventoryUpdate : public BasePacket {
    std::vector<ItemData> items;

    void serialize(Core::PacketWriter& writer) const override {
        write_vector(writer, items);
    }

    bool deserialize(Core::PacketReader& reader) override {
        return read_vector(reader, items);
    }

    uint16_t get_opcode() const override {
        return static_cast<uint16_t>(Common::Opcode::Inventory);
    }
};

struct SendZoneDetails : public BasePacket {
    uint32_t world_id{0};
    std::string world_name;
    Position spawn_position;

    void serialize(Core::PacketWriter& writer) const override {
        writer.write(world_id);
        writer.write_string(world_name);
        spawn_position.serialize(writer);
    }

    bool deserialize(Core::PacketReader& reader) override {
        return reader.read(world_id) &&
               reader.read_string(world_name) &&
               spawn_position.deserialize(reader);
    }

    uint16_t get_opcode() const override {
        return static_cast<uint16_t>(Common::Opcode::SendZoneDetails);
    }
};

} // namespace Sanctuary::Packet
