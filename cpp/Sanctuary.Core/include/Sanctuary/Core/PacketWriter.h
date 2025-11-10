#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <span>
#include <cstdint>
#include <concepts>
#include <type_traits>
#include <bit>
#include <chrono>

namespace Sanctuary::Core::IO {

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<typename T>
concept EnumType = std::is_enum_v<T>;

class PacketWriter {
public:
    PacketWriter() = default;

    [[nodiscard]] std::span<const std::byte> buffer() const noexcept {
        return std::span{buffer_};
    }

    [[nodiscard]] std::vector<std::byte> takeBuffer() noexcept {
        return std::move(buffer_);
    }

    void clear() noexcept {
        buffer_.clear();
    }

    // Write arithmetic types
    template<Arithmetic T>
    void write(T value) {
        auto bytes = std::as_bytes(std::span{&value, 1});
        buffer_.insert(buffer_.end(), bytes.begin(), bytes.end());
    }

    // Write bool
    void write(bool value) {
        buffer_.push_back(static_cast<std::byte>(value ? 1 : 0));
    }

    // Write string (length-prefixed)
    void write(std::string_view value, size_t maxLength = std::numeric_limits<size_t>::max()) {
        if (value.empty()) {
            write(int32_t{0});
            return;
        }

        if (value.size() > maxLength) {
            throw std::out_of_range("String exceeds max length");
        }

        write(static_cast<int32_t>(value.size()));
        auto bytes = std::as_bytes(std::span{value.data(), value.size()});
        buffer_.insert(buffer_.end(), bytes.begin(), bytes.end());
    }

    // Write raw bytes
    void write(std::span<const std::byte> data) {
        buffer_.insert(buffer_.end(), data.begin(), data.end());
    }

    // Write payload (length-prefixed bytes)
    void writePayload(std::span<const std::byte> data) {
        write(static_cast<int32_t>(data.size()));
        if (!data.empty()) {
            write(data);
        }
    }

    // Write Vector4
    struct Vector4 {
        float x, y, z, w;
    };

    void write(const Vector4& value, bool limited = false) {
        write(value.x);
        write(value.y);
        write(value.z);
        if (!limited) {
            write(value.w);
        }
    }

    // Write Quaternion
    struct Quaternion {
        float x, y, z, w;
    };

    void write(const Quaternion& value, bool limited = false) {
        write(value.x);
        write(value.y);
        write(value.z);
        if (!limited) {
            write(value.w);
        }
    }

    // Write DateTime/Timestamp
    void write(std::chrono::system_clock::time_point timestamp) {
        auto unixTime = std::chrono::duration_cast<std::chrono::seconds>(
            timestamp.time_since_epoch()
        ).count();
        write(static_cast<int64_t>(unixTime));
    }

    // Write enum
    template<EnumType E>
    void write(E value) {
        using UnderlyingType = std::underlying_type_t<E>;
        write(static_cast<UnderlyingType>(value));
    }

    // Write vector of arithmetic types
    template<Arithmetic T>
    void write(std::span<const T> values) {
        write(static_cast<int32_t>(values.size()));
        for (const auto& value : values) {
            write(value);
        }
    }

private:
    std::vector<std::byte> buffer_;
};

} // namespace Sanctuary::Core::IO
