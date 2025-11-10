#pragma once

#include "PacketSerializer.h"
#include <Sanctuary/Packet/Common/Opcodes.h>
#include <string>
#include <vector>
#include <cstdint>

namespace Sanctuary::Packet {

struct LoginRequest : public BasePacket {
    std::string username;
    std::string password;
    std::string version;
    uint32_t client_protocol{0};

    void serialize(Core::PacketWriter& writer) const override {
        writer.write_string(username);
        writer.write_string(password);
        writer.write_string(version);
        writer.write(client_protocol);
    }

    bool deserialize(Core::PacketReader& reader) override {
        return reader.read_string(username) &&
               reader.read_string(password) &&
               reader.read_string(version) &&
               reader.read(client_protocol);
    }

    uint16_t get_opcode() const override {
        return static_cast<uint16_t>(Common::Opcode::LoginRequest);
    }
};

struct LoginReply : public BasePacket {
    bool success{false};
    uint64_t session_id{0};
    std::string error_message;
    uint32_t user_id{0};

    void serialize(Core::PacketWriter& writer) const override {
        writer.write(success);
        writer.write(session_id);
        writer.write_string(error_message);
        writer.write(user_id);
    }

    bool deserialize(Core::PacketReader& reader) override {
        return reader.read(success) &&
               reader.read(session_id) &&
               reader.read_string(error_message) &&
               reader.read(user_id);
    }

    uint16_t get_opcode() const override {
        return static_cast<uint16_t>(Common::Opcode::LoginReply);
    }
};

struct CharacterInfo {
    uint64_t character_id{0};
    std::string name;
    uint32_t model_id{0};
    uint32_t level{0};
    uint32_t world_id{0};

    void serialize(Core::PacketWriter& writer) const {
        writer.write(character_id);
        writer.write_string(name);
        writer.write(model_id);
        writer.write(level);
        writer.write(world_id);
    }

    bool deserialize(Core::PacketReader& reader) {
        return reader.read(character_id) &&
               reader.read_string(name) &&
               reader.read(model_id) &&
               reader.read(level) &&
               reader.read(world_id);
    }
};

struct CharacterSelectInfoReply : public BasePacket {
    std::vector<CharacterInfo> characters;
    uint32_t max_characters{4};

    void serialize(Core::PacketWriter& writer) const override {
        write_vector(writer, characters);
        writer.write(max_characters);
    }

    bool deserialize(Core::PacketReader& reader) override {
        return read_vector(reader, characters) &&
               reader.read(max_characters);
    }

    uint16_t get_opcode() const override {
        return static_cast<uint16_t>(Common::Opcode::CharacterSelectInfoReply);
    }
};

struct CharacterCreateRequest : public BasePacket {
    std::string name;
    uint32_t model_id{0};
    uint32_t head_model{0};
    uint32_t hair_model{0};
    uint32_t hair_color{0};
    uint32_t eye_color{0};
    uint32_t skin_tone{0};

    void serialize(Core::PacketWriter& writer) const override {
        writer.write_string(name);
        writer.write(model_id);
        writer.write(head_model);
        writer.write(hair_model);
        writer.write(hair_color);
        writer.write(eye_color);
        writer.write(skin_tone);
    }

    bool deserialize(Core::PacketReader& reader) override {
        return reader.read_string(name) &&
               reader.read(model_id) &&
               reader.read(head_model) &&
               reader.read(hair_model) &&
               reader.read(hair_color) &&
               reader.read(eye_color) &&
               reader.read(skin_tone);
    }

    uint16_t get_opcode() const override {
        return static_cast<uint16_t>(Common::Opcode::CharacterCreateRequest);
    }
};

struct CharacterLoginRequest : public BasePacket {
    uint64_t character_id{0};
    uint32_t world_id{0};

    void serialize(Core::PacketWriter& writer) const override {
        writer.write(character_id);
        writer.write(world_id);
    }

    bool deserialize(Core::PacketReader& reader) override {
        return reader.read(character_id) &&
               reader.read(world_id);
    }

    uint16_t get_opcode() const override {
        return static_cast<uint16_t>(Common::Opcode::CharacterLoginRequest);
    }
};

} // namespace Sanctuary::Packet
