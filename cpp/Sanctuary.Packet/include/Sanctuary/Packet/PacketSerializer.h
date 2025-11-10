#pragma once

#include <span>
#include <cstdint>
#include <string>
#include <vector>
#include <concepts>
#include <Sanctuary/Core/PacketWriter.h>
#include <Sanctuary/Core/PacketReader.h>

namespace Sanctuary::Packet {

// Concept for types that can be serialized
template<typename T>
concept Serializable = requires(T t, Core::PacketWriter& writer, Core::PacketReader& reader) {
    { t.serialize(writer) } -> std::same_as<void>;
    { t.deserialize(reader) } -> std::same_as<bool>;
};

// Base packet structure
struct BasePacket {
    virtual ~BasePacket() = default;

    virtual void serialize(Core::PacketWriter& writer) const = 0;
    virtual bool deserialize(Core::PacketReader& reader) = 0;
    virtual uint16_t get_opcode() const = 0;
};

// Helper functions for serialization
template<Serializable T>
void serialize_packet(const T& packet, Core::PacketWriter& writer) {
    packet.serialize(writer);
}

template<Serializable T>
bool deserialize_packet(T& packet, Core::PacketReader& reader) {
    return packet.deserialize(reader);
}

// Vector serialization helpers
template<typename T>
void write_vector(Core::PacketWriter& writer, const std::vector<T>& vec) {
    writer.write(static_cast<uint32_t>(vec.size()));
    for (const auto& item : vec) {
        if constexpr (Serializable<T>) {
            item.serialize(writer);
        } else {
            writer.write(item);
        }
    }
}

template<typename T>
bool read_vector(Core::PacketReader& reader, std::vector<T>& vec) {
    uint32_t size;
    if (!reader.read(size)) return false;

    vec.clear();
    vec.reserve(size);

    for (uint32_t i = 0; i < size; ++i) {
        T item;
        if constexpr (Serializable<T>) {
            if (!item.deserialize(reader)) return false;
        } else {
            if (!reader.read(item)) return false;
        }
        vec.push_back(std::move(item));
    }

    return true;
}

} // namespace Sanctuary::Packet
