#pragma once

#include <span>
#include <string>
#include <string_view>
#include <cstdint>
#include <optional>
#include <bit>
#include <concepts>
#include <type_traits>
#include <stdexcept>

namespace Sanctuary::Core::IO {

// C++23 concepts for type safety
template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<typename T>
concept TrivialType = std::is_trivial_v<T>;

class PacketReader {
public:
    explicit constexpr PacketReader(std::span<const std::byte> data) noexcept
        : data_(data), position_(0) {}

    [[nodiscard]] constexpr size_t remainingLength() const noexcept {
        return data_.size() - position_;
    }

    [[nodiscard]] constexpr std::span<const std::byte> consumedSpan() const noexcept {
        return data_.subspan(0, position_);
    }

    [[nodiscard]] constexpr std::span<const std::byte> remainingSpan() const noexcept {
        return data_.subspan(position_);
    }

    constexpr void advance(size_t count) {
        if (count > remainingLength()) {
            throw std::out_of_range("PacketReader: advance beyond buffer");
        }
        position_ += count;
    }

    constexpr void rewind(size_t count) {
        if (count > position_) {
            throw std::out_of_range("PacketReader: rewind beyond start");
        }
        position_ -= count;
    }

    constexpr void reset() noexcept {
        position_ = 0;
    }

    // Read raw bytes
    [[nodiscard]] constexpr std::span<const std::byte> read(size_t count) {
        if (auto result = tryRead(count)) {
            return *result;
        }
        throw std::out_of_range("PacketReader: read beyond buffer");
    }

    [[nodiscard]] constexpr std::optional<std::span<const std::byte>> tryRead(size_t count) noexcept {
        if (count > remainingLength()) {
            return std::nullopt;
        }

        auto result = data_.subspan(position_, count);
        position_ += count;
        return result;
    }

    // Read trivial types with C++23 features
    template<TrivialType T>
    [[nodiscard]] constexpr T read() {
        if (auto result = tryRead<T>()) {
            return *result;
        }
        throw std::out_of_range("PacketReader: read type beyond buffer");
    }

    template<TrivialType T>
    [[nodiscard]] constexpr std::optional<T> tryRead() noexcept {
        if (sizeof(T) > remainingLength()) {
            return std::nullopt;
        }

        T value;
        std::memcpy(&value, data_.data() + position_, sizeof(T));
        position_ += sizeof(T);
        return value;
    }

    // Endianness-aware reads using C++23 std::byteswap
    template<Arithmetic T>
    [[nodiscard]] constexpr std::optional<T> tryReadEndian(bool isLittleEndian = false) noexcept {
        auto result = tryRead<T>();
        if (!result) {
            return std::nullopt;
        }

        if constexpr (sizeof(T) > 1) {
            if (isLittleEndian != (std::endian::native == std::endian::little)) {
                return std::byteswap(*result);
            }
        }

        return result;
    }

    // Read string (length-prefixed)
    [[nodiscard]] std::optional<std::string> tryReadString() noexcept {
        auto length = tryRead<int32_t>();
        if (!length) {
            return std::nullopt;
        }

        if (*length == 0) {
            return std::string{};
        }

        if (*length < 0 || static_cast<size_t>(*length) > remainingLength()) {
            return std::nullopt;
        }

        auto bytes = tryRead(*length);
        if (!bytes) {
            return std::nullopt;
        }

        return std::string(reinterpret_cast<const char*>(bytes->data()), bytes->size());
    }

    // Read Vector4 (4 floats)
    struct Vector4 {
        float x, y, z, w;
    };

    [[nodiscard]] std::optional<Vector4> tryReadVector4(bool limited = false) noexcept {
        Vector4 result{};

        if (auto x = tryRead<float>()) result.x = *x; else return std::nullopt;
        if (auto y = tryRead<float>()) result.y = *y; else return std::nullopt;
        if (auto z = tryRead<float>()) result.z = *z; else return std::nullopt;

        if (limited) {
            result.w = 1.0f;
        } else {
            if (auto w = tryRead<float>()) result.w = *w; else return std::nullopt;
        }

        return result;
    }

    // Read Quaternion (4 floats)
    struct Quaternion {
        float x, y, z, w;
    };

    [[nodiscard]] std::optional<Quaternion> tryReadQuaternion(bool limited = false) noexcept {
        Quaternion result{};

        if (auto x = tryRead<float>()) result.x = *x; else return std::nullopt;
        if (auto y = tryRead<float>()) result.y = *y; else return std::nullopt;
        if (auto z = tryRead<float>()) result.z = *z; else return std::nullopt;

        if (limited) {
            result.w = 0.0f;
        } else {
            if (auto w = tryRead<float>()) result.w = *w; else return std::nullopt;
        }

        return result;
    }

private:
    std::span<const std::byte> data_;
    size_t position_;
};

} // namespace Sanctuary::Core::IO
